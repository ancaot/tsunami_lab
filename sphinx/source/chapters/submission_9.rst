#############################
Submission 9: Paralleliserung
#############################

1. OpenMP Paralleliserung
=========================

Wir haben den zweidimensionalen Solver mit OpenMP parallelisiert. Die
Parallelisierung liegt bewusst im ``WavePropagation2d``-Patch, weil dort die
meiste Zeit verbracht wird und weil die Schleifenstruktur dort groß genug ist,
damit Threading nicht nur Overhead erzeugt.

Im Build-System wurde dafür eine neue Option eingeführt:

.. code-block:: bash

    scons mode=release opt=O3 openmp=true

Bei GCC und Clang fügt SCons dadurch ``-fopenmp`` zu Compile- und Link-Flags
hinzu. Bei MSVC wird ``/openmp`` verwendet. Zum Vergleich kann OpenMP auch
abgeschaltet werden:

.. code-block:: bash

    scons mode=release opt=O3 openmp=false

Die wichtigsten Änderungen im Solver sind:

* Die alten Werte werden vor jedem Sweep parallel in das neue Array kopiert.
* Die x-Richtung wird zeilenweise parallelisiert.
* Die y-Richtung wird spaltenweise parallelisiert.
* Die Ghost-Zellen werden ebenfalls parallel gesetzt.
* Die Initialisierung der großen Arrays erfolgt parallel. Damit wird auf NUMA-Systemen auch das First-Touch-Prinzip unterstützt, wenn die Initialisierung auf Grace mit der finalen Thread-/Pinning-Konfiguration läuft.
* Die OpenMP-Schleifen verwenden ``schedule(runtime)``, damit wir die Scheduling-Strategie ueber ``OMP_SCHEDULE`` messen können.

Beim Umbau ist uns außerdem aufgefallen, dass die alte Hilfsfunktion
``initialiseArrays`` Pointer nur by value bekommen hat. Dadurch wurden die
``old``- und ``new``-Pointer außerhalb der Funktion nicht wirklich
umgesetzt. Fuer die OpenMP-Version wurde das durch eine klare
``copyOldToNew``-Funktion ersetzt.

Lokal wurde der Test-Binary einmal mit und einmal ohne OpenMP gebaut. Beide
Varianten bestanden die Tests:

.. code-block:: text

    All tests passed (10136 assertions in 18 test cases)


2. Parallelisiert vs Seriell
============================

Die echten Speedup-Werte müssen auf NVIDIA Grace gemessen werden, weil unser
lokaler Rechner nicht dieselbe Kernzahl, Speicherbandbreite und NUMA-Struktur
hat. Für die Messung verwenden wir immer dieselbe Konfiguration und variieren
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
      - 10 minutes, 0 seconds, 303 milliseconds, 278 microseconds, 783 nanoseconds
      - 1.00
      - 954238
    * - 2
      - 3 minutes, 31 seconds, 807 milliseconds, 434 microseconds, 454 nanoseconds
      - 2.83419
      - 2.70449e+06
    * - 4
      - 3 minutes, 24 seconds, 822 milliseconds, 681 microseconds, 491 nanoseconds
      - 2.93084
      - 2.79672e+06
    * - 8
      - 2 minutes, 27 seconds, 239 milliseconds, 14 microseconds, 384 nanoseconds
      - 4.07707
      - 3.89049e+06
    * - 16
      - 2 minutes, 19 seconds, 356 milliseconds, 373 microseconds, 858 nanoseconds
      - 4.30768
      - 4.11055e+06
    * - 32
      - 2 minutes, 3 seconds, 648 milliseconds, 891 microseconds, 921 nanoseconds
      - 4.85490
      - 4.63273e+06
    * - 64
      - 1 minutes, 59 seconds, 200 milliseconds, 837 microseconds, 561 nanoseconds
      - 5.03606
      - 4.8056e+06
    * - 96
      - 3 minutes, 21 seconds, 457 milliseconds, 676 microseconds, 385 nanoseconds
      - 2.97980
      - 2.84344e+06
    * - 128
      - 3 minutes, 30 seconds, 671 milliseconds, 700 microseconds, 630 nanoseconds
      - 2.84947
      - 2.71907e+06
    * - 144
      - 3 minutes, 27 seconds, 226 milliseconds, 815 microseconds, 971 nanoseconds
      - 2.89684
      - 2.76428e+06

Wir erwarten keinen idealen Speedup bis 144 Threads. Gründe dafür sind die
seriellen Teile außerhalb des Zeitschritt-Kerns, Speicherbandbreite,
Thread-Overhead, Ghost-Zell-Behandlung und Datei-I/O.


3. 2-dimensionaler Solver
=========================

Beim 2D-Solver ist die Wahl der Schleife wichtig. Innerhalb einer Zeile teilen
sich benachbarte Kanten dieselbe Zelle. Wenn man diese innere Kantenschleife
parallelisiert, koennen zwei Threads gleichzeitig dieselbe Höhe oder denselben
Impuls aktualisieren. Das wäre eine Race Condition.

Besser ist deshalb:

* Im x-Sweep parallelisieren wir die äußere Schleife über die y-Zeilen.
  Jede Thread-Zeile schreibt nur in ihre eigene Zeile.
* Im y-Sweep parallelisieren wir die äußere Schleife über die x-Spalten.
  Jede Thread-Spalte schreibt nur in ihre eigene Spalte.

Damit bleiben die Updates innerhalb einer Zeile bzw. Spalte seriell, aber die
unabhängigen Zeilen bzw. Spalten können gleichzeitig berechnet werden. Diese
Variante ist weniger spektakulär als einfach ``collapse(2)`` auf alle
Schleifen zu schreiben, aber sie ist korrekt.


4. Scheduling und Pinning Strategien
====================================

Für unsere Schleifen ist ``schedule(static)`` der natürliche Startpunkt. Die
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

* ``static`` ist am besten oder sehr nah am besten, weil die Last gleichmäßig ist.
* ``close`` kann bei kleineren Threadzahlen helfen, weil Daten lokal bleiben.
* ``spread`` kann bei hohen Threadzahlen besser sein, weil mehr Speichercontroller genutzt werden.
* NUMA-Effekte sieht man besonders bei 96 bis 144 Threads. Darum initialisieren wir die großen Arrays parallel, sodass First Touch zur späteren Thread-Platzierung passt.

Das Skript ``tools/run_openmp_grace_benchmark.sh`` führt diese Kombinationen
automatisch aus und schreibt pro Lauf eine Logdatei nach
``outputs/omp_benchmarks``.

Folgende Tabellen zeigen die Laufzeiten der Zeitschritt-Schleifen für die Thread-Counts 1, 64 und 144. 
Die Simulation war wie zuvor Tohoku 2500m Auflösung.

.. list-table:: Vergleich der Ergebnisse aus Logdateien (Thread-Count = 1)
    :header-rows: 1

    * - OMP_SCHEDULE
      - OMP_PROC_BIND
      - Zeitschritt-Schleife
      - Programm
    * - static
      - closed
      - 16 seconds, 311 milliseconds, 185 microseconds, 258 nanoseconds
      - 21 seconds, 380 milliseconds, 990 microseconds, 998 nanoseconds
    * - static
      - spread
      - 16 seconds, 26 milliseconds, 513 microseconds, 329 nanoseconds
      - 21 seconds, 95 milliseconds, 314 microseconds, 661 nanoseconds
    * - dynamic
      - closed
      - 1 minutes, 0 seconds, 393 milliseconds, 550 microseconds, 266 nanoseconds
      - 1 minutes, 11 seconds, 73 milliseconds, 34 microseconds, 134 nanoseconds
    * - dynamic
      - spread
      - 31 seconds, 802 milliseconds, 244 microseconds, 671 nanoseconds
      - 36 seconds, 956 milliseconds, 647 microseconds, 372 nanoseconds
    * - guided
      - closed
      - 21 seconds, 376 milliseconds, 761 microseconds, 378 nanoseconds
      - 28 seconds, 18 milliseconds, 528 microseconds, 760 nanoseconds
    * - guided
      - spread
      - 16 seconds, 315 milliseconds, 959 microseconds, 784 nanoseconds
      - 21 seconds, 992 milliseconds, 328 microseconds, 918 nanoseconds

.. list-table:: Vergleich der Ergebnisse aus Logdateien (Thread-Count = 64)
    :header-rows: 1

    * - OMP_SCHEDULE
      - OMP_PROC_BIND
      - Zeitschritt-Schleife
      - Programm
    * - static
      - closed
      - 966 milliseconds, 892 microseconds, 330 nanoseconds
      - 6 seconds, 44 milliseconds, 880 microseconds, 455 nanoseconds
    * - static
      - spread
      - 1 seconds, 183 milliseconds, 129 microseconds, 710 nanoseconds
      - 7 seconds, 488 milliseconds, 757 microseconds, 408 nanoseconds
    * - dynamic
      - closed
      - 1 minutes, 31 seconds, 871 milliseconds, 674 microseconds, 929 nanoseconds
      - 1 minutes, 36 seconds, 945 milliseconds, 625 microseconds, 75 nanoseconds
    * - dynamic
      - spread
      - 2 minutes, 58 seconds, 532 milliseconds, 602 microseconds, 1 nanoseconds
      - 3 minutes, 4 seconds, 42 milliseconds, 649 microseconds, 606 nanoseconds
    * - guided
      - closed
      - 1 seconds, 200 milliseconds, 408 microseconds, 76 nanoseconds
      - 6 seconds, 276 milliseconds, 176 microseconds, 484 nanoseconds
    * - guided
      - spread
      - 1 seconds, 901 milliseconds, 54 microseconds, 349 nanoseconds
      - 7 seconds, 152 milliseconds, 32 microseconds, 83 nanoseconds

.. list-table:: Vergleich der Ergebnisse aus Logdateien (Thread-Count = 144)
    :header-rows: 1

    * - OMP_SCHEDULE
      - OMP_PROC_BIND
      - Zeitschritt-Schleife
      - Programm
    * - static
      - closed
      - 8 seconds, 163 milliseconds, 429 microseconds, 861 nanoseconds
      - 13 seconds, 244 milliseconds, 825 microseconds, 175 nanoseconds
    * - static
      - spread
      - 31 seconds, 310 milliseconds, 631 microseconds, 914 nanoseconds
      - 41 seconds, 998 milliseconds, 880 microseconds, 160 nanoseconds
    * - dynamic
      - closed
      - 4 minutes, 24 seconds, 998 milliseconds, 81 microseconds, 757 nanoseconds
      - 4 minutes, 30 seconds, 524 milliseconds, 594 microseconds, 513 nanoseconds
    * - dynamic
      - spread
      - 3 minutes, 20 seconds, 935 milliseconds, 712 microseconds, 388 nanoseconds
      - 3 minutes, 26 seconds, 21 milliseconds, 435 microseconds, 654 nanoseconds
    * - guided
      - closed
      - 6 seconds, 416 milliseconds, 452 microseconds, 229 nanoseconds
      - 11 seconds, 496 milliseconds, 478 microseconds, 448 nanoseconds
    * - guided
      - spread
      - 6 seconds, 585 milliseconds, 191 microseconds, 227 nanoseconds
      - 11 seconds, 672 milliseconds, 818 microseconds, 607 nanoseconds

Generell war ``static`` bei niedriegeren Thread-Counts die beste Option für die Laufzeit. 
Bei 144 Threads war jedoch ``guided`` schneller. 
``dynamic`` war generell am langsamsten. 

``spread`` im Vergleich zu ``close`` schien sich abhängig von der Schedule Art schneller oder langsamer zu verhalten. 


5. Benchmark vom parallelisierten Solver
========================================

Für den optionalen Benchmark nutzen wir:

* 2011 M 9.1 Tohoku Event
* 250m Auflösung
* 10000 Zeitschritte
* Datei-I/O alle 100 Zeitschritte
* maximal 144 Threads auf Grace

Damit diese Vorgabe direkt über die Konfiguration steuerbar ist, wurden zwei
optionale Keys ergänzt:

.. code-block:: json

    "time_steps": 10000,
    "output_interval": 100

Wenn ``time_steps`` gesetzt ist, läuft die Simulation genau diese Anzahl von
Zeitschritten. Ohne diesen Key wird weiterhin wie bisher bis zur konfigurierten
Endzeit gerechnet. Ohne ``output_interval`` bleibt der alte Standard von 25
Zeitschritten erhalten.

Das Programm gibt am Ende neben der Laufzeit der Zeitschritt-Schleife auch
``Cell updates/s`` aus. Die Zahl berechnet sich als:

.. math::

    \frac{n_x \cdot n_y \cdot n_\text{steps}}{T_\text{loop}}

Unser Teamname für die Benchmark-Liste:

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
