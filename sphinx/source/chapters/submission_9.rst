#############################
Submission 9: Paralleliserung
#############################

1. OpenMP Paralleliserung
=========================

Wir haben den zweidimensionalen Solver mit OpenMP parallelisiert. Die
Parallelisierung liegt bewusst im ``WavePropagation2d``-Patch, weil dort die
meiste Zeit verbracht wird und weil die Schleifenstruktur dort gross genug ist,
damit Threading nicht nur Overhead erzeugt.

Im Build-System wurde dafuer eine neue Option eingefuehrt:

.. code-block:: bash

    scons mode=release opt=O3 openmp=true

Bei GCC und Clang fuegt SCons dadurch ``-fopenmp`` zu Compile- und Link-Flags
hinzu. Bei MSVC wird ``/openmp`` verwendet. Zum Vergleich kann OpenMP auch
abgeschaltet werden:

.. code-block:: bash

    scons mode=release opt=O3 openmp=false

Die wichtigsten Aenderungen im Solver sind:

* Die alten Werte werden vor jedem Sweep parallel in das neue Array kopiert.
* Die x-Richtung wird zeilenweise parallelisiert.
* Die y-Richtung wird spaltenweise parallelisiert.
* Die Ghost-Zellen werden ebenfalls parallel gesetzt.
* Die Initialisierung der grossen Arrays erfolgt parallel. Damit wird auf NUMA-Systemen auch das First-Touch-Prinzip unterstuetzt, wenn die Initialisierung auf Grace mit der finalen Thread-/Pinning-Konfiguration laeuft.
* Die OpenMP-Schleifen verwenden ``schedule(runtime)``, damit wir die Scheduling-Strategie ueber ``OMP_SCHEDULE`` messen koennen.

Beim Umbau ist uns ausserdem aufgefallen, dass die alte Hilfsfunktion
``initialiseArrays`` Pointer nur by value bekommen hat. Dadurch wurden die
``old``- und ``new``-Pointer ausserhalb der Funktion nicht wirklich
umgesetzt. Fuer die OpenMP-Version wurde das durch eine klare
``copyOldToNew``-Funktion ersetzt.

Lokal wurde der Test-Binary einmal mit und einmal ohne OpenMP gebaut. Beide
Varianten bestanden die Tests:

.. code-block:: text

    All tests passed (10136 assertions in 18 test cases)


2. Parallelisiert vs Seriell
============================

Die echten Speedup-Werte muessen auf NVIDIA Grace gemessen werden, weil unser
lokaler Rechner nicht dieselbe Kernzahl, Speicherbandbreite und NUMA-Struktur
hat. Fuer die Messung verwenden wir immer dieselbe Konfiguration und variieren
nur die OpenMP-Umgebung.

Serielle Referenz:

.. code-block:: bash

    scons --clean
    scons mode=release opt=O3 openmp=false netcdf=on
    ./build/tsunami_lab

Parallele Messung:

.. code-block:: bash

    scons --clean
    scons mode=release opt=O3 openmp=true netcdf=on
    OMP_NUM_THREADS=1 ./build/tsunami_lab
    OMP_NUM_THREADS=2 ./build/tsunami_lab
    OMP_NUM_THREADS=4 ./build/tsunami_lab
    OMP_NUM_THREADS=8 ./build/tsunami_lab
    OMP_NUM_THREADS=16 ./build/tsunami_lab
    OMP_NUM_THREADS=32 ./build/tsunami_lab
    OMP_NUM_THREADS=64 ./build/tsunami_lab
    OMP_NUM_THREADS=96 ./build/tsunami_lab
    OMP_NUM_THREADS=128 ./build/tsunami_lab
    OMP_NUM_THREADS=144 ./build/tsunami_lab

Der Speedup wird mit

.. math::

    S_p = \frac{T_1}{T_p}

berechnet. ``T_1`` ist dabei die Laufzeit mit einem Thread, ``T_p`` die
Laufzeit mit ``p`` Threads.

.. list-table:: Messwerte auf Grace
    :header-rows: 1

    * - Threads
      - Zeit Zeitschritt-Schleife
      - Speedup
      - Cell updates/s
    * - 1
      - auf Grace eintragen
      - 1.00
      - auf Grace eintragen
    * - 2
      - auf Grace eintragen
      - auf Grace berechnen
      - auf Grace eintragen
    * - 4
      - auf Grace eintragen
      - auf Grace berechnen
      - auf Grace eintragen
    * - 8
      - auf Grace eintragen
      - auf Grace berechnen
      - auf Grace eintragen
    * - 16
      - auf Grace eintragen
      - auf Grace berechnen
      - auf Grace eintragen
    * - 32
      - auf Grace eintragen
      - auf Grace berechnen
      - auf Grace eintragen
    * - 64
      - auf Grace eintragen
      - auf Grace berechnen
      - auf Grace eintragen
    * - 96
      - auf Grace eintragen
      - auf Grace berechnen
      - auf Grace eintragen
    * - 128
      - auf Grace eintragen
      - auf Grace berechnen
      - auf Grace eintragen
    * - 144
      - auf Grace eintragen
      - auf Grace berechnen
      - auf Grace eintragen

Wir erwarten keinen idealen Speedup bis 144 Threads. Gruende dafuer sind die
seriellen Teile ausserhalb des Zeitschritt-Kerns, Speicherbandbreite,
Thread-Overhead, Ghost-Zell-Behandlung und Datei-I/O.


3. 2-dimensionaler Solver
=========================

Beim 2D-Solver ist die Wahl der Schleife wichtig. Innerhalb einer Zeile teilen
sich benachbarte Kanten dieselbe Zelle. Wenn man diese innere Kantenschleife
parallelisiert, koennen zwei Threads gleichzeitig dieselbe Hoehe oder denselben
Impuls aktualisieren. Das waere eine Race Condition.

Besser ist deshalb:

* Im x-Sweep parallelisieren wir die aeussere Schleife ueber die y-Zeilen.
  Jede Thread-Zeile schreibt nur in ihre eigene Zeile.
* Im y-Sweep parallelisieren wir die aeussere Schleife ueber die x-Spalten.
  Jede Thread-Spalte schreibt nur in ihre eigene Spalte.

Damit bleiben die Updates innerhalb einer Zeile bzw. Spalte seriell, aber die
unabhaengigen Zeilen bzw. Spalten koennen gleichzeitig berechnet werden. Diese
Variante ist weniger spektakulaer als einfach ``collapse(2)`` auf alle
Schleifen zu schreiben, aber sie ist korrekt.


4. Scheduling und Pinning Strategien
====================================

Fuer unsere Schleifen ist ``schedule(static)`` der natuerliche Startpunkt. Die
Arbeit pro Zeile bzw. Spalte ist nahezu gleich, deshalb bringt dynamisches
Scheduling wahrscheinlich mehr Overhead als Nutzen.

Auf Grace testen wir trotzdem mehrere Varianten:

.. code-block:: bash

    export OMP_PLACES=cores
    export OMP_PROC_BIND=close
    export OMP_SCHEDULE=static

und zum Vergleich:

.. code-block:: bash

    export OMP_PROC_BIND=spread
    export OMP_SCHEDULE=dynamic
    export OMP_SCHEDULE=guided

Unsere Erwartung:

* ``static`` ist am besten oder sehr nah am besten, weil die Last gleichmaessig ist.
* ``close`` kann bei kleineren Threadzahlen helfen, weil Daten lokal bleiben.
* ``spread`` kann bei hohen Threadzahlen besser sein, weil mehr Speichercontroller genutzt werden.
* NUMA-Effekte sieht man besonders bei 96 bis 144 Threads. Darum initialisieren wir die grossen Arrays parallel, sodass First Touch zur spaeteren Thread-Platzierung passt.

Das Skript ``tools/run_openmp_grace_benchmark.sh`` fuehrt diese Kombinationen
automatisch aus und schreibt pro Lauf eine Logdatei nach
``outputs/omp_benchmarks``.


5. Benchmark vom parallelisierten Solver
========================================

Fuer den optionalen Benchmark nutzen wir:

* 2011 M 9.1 Tohoku Event
* 250m Aufloesung
* 10000 Zeitschritte
* Datei-I/O alle 100 Zeitschritte
* maximal 144 Threads auf Grace

Damit diese Vorgabe direkt ueber die Konfiguration steuerbar ist, wurden zwei
optionale Keys ergaenzt:

.. code-block:: json

    "time_steps": 10000,
    "output_interval": 100

Wenn ``time_steps`` gesetzt ist, laeuft die Simulation genau diese Anzahl von
Zeitschritten. Ohne diesen Key wird weiterhin wie bisher bis zur konfigurierten
Endzeit gerechnet. Ohne ``output_interval`` bleibt der alte Standard von 25
Zeitschritten erhalten.

Das Programm gibt am Ende neben der Laufzeit der Zeitschritt-Schleife auch
``Cell updates/s`` aus. Die Zahl berechnet sich als:

.. math::

    \frac{n_x \cdot n_y \cdot n_\text{steps}}{T_\text{loop}}

Unser Teamname fuer die Benchmark-Liste:

.. code-block:: text

    Wavefront Witnesses

Die finalen Grace-Zahlen tragen wir nach dem Lauf hier ein:

.. list-table:: Optionaler Grace-Benchmark
    :header-rows: 1

    * - Team
      - Threads
      - Laufzeit
      - Cell updates/s
      - Beste Strategie
    * - Wavefront Witnesses
      - 144
      - auf Grace eintragen
      - auf Grace eintragen
      - auf Grace eintragen
