#########################
Submission 8: Optimierung
#########################

Diese Woche familisieren wir uns mit den Draco Cluster. 
Wir vergleichen unsere Simulation und ihre Performance unter verschieden Umständen und mit Nutzung verschiedener Compiler.
Dazu sollen wir untersuchen, wo in unserem Code Bottlenecks und Hotspots sind, die wir optimieren können.

8.1 Draco
=========

1. Timer für Zeitschritt-Schleife
---------------------------------

Wir dachten, am besten wäre es denn Timer am Anfang zu implementieren um bei den verschiedenen Simulationen direkt schon die Zeit dabei zu haben. 
So müssen wir nicht dieselben Simulationen mehrfach auf dem eigenen Computer oder auf dem Cluster durchführen. 

Mithilfe der ``std::chrono::high_resolution_clock::now()`` berechnen wir die Zeit, wie lange die Zeitschritt-Schleife dauerte und wie lange 
die gesamte Berechnung inklusive Initialisierung der Zellen dauert. 
Die Zeit messen wir in Nanosekunden, um so genau wie möglich zu messen.

Desweiteren haben wir eine Funktion ``printDuration(std::chrono::nanoseconds duration)`` eingeführt, die uns die Zeitlänge auch in Stunden, Minuten etc. ausgibt und nicht nur in Nanosekunden.

Die Setup-Zeit war zwar nicht in der Aufgabe gefragt gewesen, aber wir hatten persönliches Interesse an wie lange diese dauern kann.

Dadurch, dass wir die Zeit vor den ersten Kompilierungen auf den Clustern implementiert haben, befindet sich der zeitliche Vergleich 
sowie die interaktiven und batch Jobs im nächsten Abschnitt.

2. Interaktive und batch Jobs
-----------------------------

Für die interaktiven und batch Jobs wurden die Simulationen von dem ``tsunami_lab`` Ordner ausgeführt. 
Demnach ist auch das batch-Skript entstanden. 
Um das Ausführen der Simulation mit dem batch-Skript zu vereinfachen, haben wir eine boolean Variable eingeführt, um 
die Launch-Settings Eingabe zu überspringen. 
Wir ändern die Konfiguration für die batch-Jobs nicht. 

Wir haben verschiedene Szenarien ausprobiert, um zu testen, dass wir die gleichen Resultate auf dem Cluster bekommen, wie bei vorherigen Simulationen. 

Dafür überprüften wir zuerst Tohoku mit 3500m-Auflösung.
Diese ist eine sehr großschrittige Simulation und braucht nicht zuviel Zeit bei der Berechnung.

**Tohoku 3500m Auflösung**

Die ``config.json`` Datei, die für diese Simulation genutzt wurde (wie vorher sind die Input-Dateien im angezeigten Pfad gespeichert):

.. code-block:: json

    {
        "numerical_solver" : "fwave",
        "scenario" :  "tsunamievent2d",
        "wave_model" : "2d",
        "domain_size_x" : 2700000,
        "domain_size_y" : 1500000,
        "cells_x" : 771,
        "cells_y" : 429,
        "coarse_factor" : 4,
        "origin_x" : -200000,
        "origin_y" : -750000,
        "simulation_end_time" : 3600,
        "output_format" : "netcdf",
        "bathymetry_file" : "data/nc/data_in/output/tohoku_gebco20_ucsb3_250m_bath.nc",
        "displacement_file" : "data/nc/data_in/output/tohoku_gebco20_ucsb3_250m_displ.nc",
        "output_name": "tohoku_3500m_coarse_k4.nc",
        "reflective_boundary" : false,
        "initial_momentum_x" : 0,
        "initial_momentum_y": 0.0,
        "left_height": 25,
        "right_height": 55,
        "dam_location" : 0
    }

*Vergleichstabelle von persönlichen Computer, interaktiven und batch Jobs*

.. list-table::
    :header-rows: 1

    * - Information
      - Persönlicher Computer
      - Interaktiver Job
      - Batch Job
    * - cells
      - 771 x 429
      - 771 x 429
      - 771 x 429
    * - cell width
      - 3501.95 m
      - 3501.95 m
      - 3501.95 m
    * - time step
      - 5.72301 seconds
      - 5.72301 seconds
      - 5.72304 seconds
    * - steps
      - 630
      - 630
      - 630
    * - Duration of time step loop
      - 5 s, 726 ms, 966 µs, 700 ns
      - 22 s, 548 ms, 348 µs, 261 ns
      - 21 s, 717 ms, 561 µs, 433 ns
    * - Duration of programm
      - 7 s, 300 ms, 386 µs, 0 ns
      - 28 s, 772 ms, 981 µs, 698 ns
      - 26 s, 621 ms, 999 µs, 269 ns
     
Es war überraschend, dass auf meinem persönlichen Computer die Simulationen wesentlich schneller waren als auf dem Cluster. 
Dabei ist aber zu bedenken, dass wir noch keine Parallelisierung hinzugefügt hatten oder andere Optimierungen. 
Spannend ist auch, dass die Simulation über einen batch-Job schneller ging, als ein interaktiver Job. 
Dabei wird bei der Zeit nur die tatsächliche Berechnung der Simulation gemessen und nichts von der Konfiguration.

Hier nochmal die Visualisierung von der berechnenten Simulation:

.. raw:: html

   <video src="../_static/tohoku_3500_personalPC.mp4" controls style="width: 72%; max-width: 760px; display: block; margin: 1rem auto;"></video>

Visualisierung der Daten berechnet mit persönlichen Computer. 
Die Visualiserung der Daten, die das Cluster überliefert hat, war identisch, demnach entsprechen auch die Werte.


**Chile 2500m Auflösung**

.. code-block:: json

    {
        "numerical_solver" : "fwave",
        "scenario" :  "tsunamievent2d",
        "wave_model" : "2d",
        "domain_size_x" : 3500000,
        "domain_size_y" : 2950000,
        "cells_x" : 1400,
        "cells_y" : 1180,
        "coarse_factor" : 1,
        "origin_x" : -3000000,
        "origin_y" : -1500000,
        "simulation_end_time" : 3600,
        "output_format" : "netcdf",
        "bathymetry_file" : "data/nc/data_in/output/chile_gebco20_usgs_250m_bath_fixed.nc",
        "displacement_file" : "data/nc/data_in/output/chile_gebco20_usgs_250m_displ_fixed.nc",
        "output_name": "chile_2500m_coarse_k1.nc",
        "reflective_boundary" : false,
        "initial_momentum_x" : 0,
        "initial_momentum_y": 0.0,
        "left_height": 25,
        "right_height": 55,
        "dam_location" : 0
    }

*Vergleichstabelle von persönlichen Computer, interaktiven und batch Jobs*

.. list-table::
    :header-rows: 1

    * - Information
      - Persönlicher Computer
      - Interaktiver Job
      - Batch Job
    * - cells
      - 1400 x 1180
      - 1400 x 1180
      - 1400 x 1180
    * - cell width
      - 2500 m
      - 2500 m
      - 2500 m
    * - time step
      - 4.40831 seconds
      - 4.40831 seconds
      - 4.40831 seconds
    * - steps
      - 817
      - 817
      - 817
    * - Duration of time step loop
      - 0 min, 38 s, 144 ms, 639 µs, 300 ns
      - 2 min, 21 seconds, 69 milliseconds, 896 microseconds, 165 nanoseconds
      - 1 min, 50 seconds, 624 milliseconds, 224 microseconds, 284 nanoseconds
    * - Duration of programm
      - 0 min, 49 s, 821 ms, 237 µs, 600 ns
      - 3 min, 5 s, 65 ms, 49 µs, 37 ns
      - 2 min, 29 s, 349 ms, 708 µs, 569 ns
     
Auch hier war mein persönlicher Computer wesentlich schneller als das Cluster. Genauso war der batch-Job schneller als der interaktive. 

Hier nochmal die Visualisierung von der berechnenten Simulation:

.. raw:: html

   <video src="../_static/chile_2500_personalPC.mp4" controls style="width: 72%; max-width: 760px; display: block; margin: 1rem auto;"></video>

Auch hier war die Visualiserung der Dateien von den interaktiven und batch Jobs identisch zu der Ausgabe vom persönlichen PC.

**Erkenntnisse**

In diesem Fall war der persönliche Computer immer am schnellsten, danach kam der batch-Job und zuletzt der interaktive Job. 
Ob diese Zeiten bei weiter Optimierung und Parallelisierung bei behalten werden, bezweifle ich, aufgrund der Tatsache, 
dass die Cluster auf Parallelisierung ausgelegt sind oder zumindest größeren Berechnungen. 
Da eine große Auflösung untersucht wurde, wissen wir nicht wie sich die Rechner bei kleinerer Auflösung verhalten. 
Anhand den Unterschieden zwischen Tohoku und Chile lässt sich aber erschließen, dass die Cluster weiterhin deutlich länger gebraucht hätten 
als der persönliche Computer.

8.2 Compiler
============

1. Generische Compiler
----------------------

In diesem Teil vergleichen wir zwei verschiedene C++ Compiler: den GNU Compiler ``g++`` aus der GNU Compiler Collection und den LLVM/Clang Compiler ``clang++``.
Beide Compiler können C++ Code optimieren, aber sie treffen dabei teilweise unterschiedliche Entscheidungen. Deshalb kann derselbe Quellcode je nach Compiler unterschiedlich schnelle Programme erzeugen.
Die offiziellen Projektseiten sind:

* GNU Compiler Collection: https://gcc.gnu.org/
* Clang / LLVM: https://clang.llvm.org/

Damit wir unseren Code flexibel mit beiden Compilern übersetzen können, haben wir unser ``SConstruct`` erweitert.
Der Compiler kann nun über die Umgebungsvariable ``CXX`` ausgewählt werden. Dafür wird die Umgebung mit ``os.environ.copy()`` an die SCons-Umgebung weitergegeben.
Wenn ``CXX`` in der Umgebung gesetzt ist, wird der lokale SCons-Wert ``CXX`` entsprechend ersetzt:

.. code-block:: python

    env = Environment( variables = vars,
                       ENV = os.environ.copy() )

    if 'CXX' in os.environ:
      env.Replace( CXX = os.environ['CXX'] )

Damit kann ein Build mit GNU zum Beispiel so gestartet werden:

.. code-block:: bash

    CXX=g++ scons

Ein Build mit Clang funktioniert entsprechend mit:

.. code-block:: bash

    CXX=clang++ scons

Zusätzlich gibt das Build-Script den ausgewählten Compiler aus. Dadurch sieht man direkt beim Kompilieren, ob wirklich der gewünschte Compiler verwendet wurde.

2. GNU vs Clang
---------------

Für den Vergleich sollen beide Compiler auf dem Cluster mit derselben Simulationskonfiguration getestet werden.
Wichtig ist dabei, dass der Solver nicht auf den Login Nodes ausgeführt wird, sondern nur in einem interaktiven Job oder Batch Job auf einem Compute Node.
Damit die Laufzeiten vergleichbar bleiben, verwenden wir für alle Messungen dieselbe Konfiguration, zum Beispiel die Tohoku-Simulation mit 3500m-Auflösung aus Abschnitt 8.1.

Der Ablauf für GNU ist:

.. code-block:: bash

    scons -c
    CXX=g++ scons opt=O3
    srun ./build/tsunami_lab

Der Ablauf für Clang ist:

.. code-block:: bash

    scons -c
    CXX=clang++ scons opt=O3
    srun ./build/tsunami_lab

Gemessen wird vor allem die Dauer der Zeitschritt-Schleife, da diese den eigentlichen Rechenaufwand besser beschreibt als die gesamte Programmlaufzeit inklusive Setup und Ein-/Ausgabe.

.. list-table::
    :header-rows: 1

    * - Compiler
      - Optimierung
      - Duration of time step loop
      - Duration of program
      - Bemerkung
    * - ``g++``
      - ``-O3``
      - wird nach Clusterlauf eingetragen
      - wird nach Clusterlauf eingetragen
      - GNU Compiler
    * - ``clang++``
      - ``-O3``
      - wird nach Clusterlauf eingetragen
      - wird nach Clusterlauf eingetragen
      - LLVM/Clang Compiler

Aus diesen Messungen lässt sich anschliessend ableiten, welcher Compiler fuer unsere konkrete Implementierung schnelleren Code erzeugt.
Da Compileroptimierungen stark vom Code, von der CPU und von den verwendeten Flags abhängen, erwarten wir nicht automatisch, dass ein Compiler immer schneller ist.

3. Optimierungs-Switches
------------------------

Um verschiedene Optimierungslevel einfacher testen zu können, wurde im ``SConstruct`` eine neue Option ``opt`` hinzugefügt.
Diese Option kann die Werte ``O0``, ``O1``, ``O2``, ``O3`` und ``Ofast`` annehmen.
Für Release-Builds wird daraus automatisch das passende Compiler-Flag erzeugt, zum Beispiel ``-O2`` oder ``-Ofast``.

Beispiele für die Messungen:

.. code-block:: bash

    scons -c
    CXX=g++ scons opt=O2
    srun ./build/tsunami_lab

    scons -c
    CXX=g++ scons opt=Ofast
    srun ./build/tsunami_lab

    scons -c
    CXX=clang++ scons opt=O2
    srun ./build/tsunami_lab

    scons -c
    CXX=clang++ scons opt=Ofast
    srun ./build/tsunami_lab

Die Messergebnisse können dann in folgender Tabelle verglichen werden:

.. list-table::
    :header-rows: 1

    * - Compiler
      - ``-O2``
      - ``-O3``
      - ``-Ofast``
    * - ``g++``
      - wird nach Clusterlauf eingetragen
      - wird nach Clusterlauf eingetragen
      - wird nach Clusterlauf eingetragen
    * - ``clang++``
      - wird nach Clusterlauf eingetragen
      - wird nach Clusterlauf eingetragen
      - wird nach Clusterlauf eingetragen

Bei numerischen Simulationen muss man bei Optimierungsflags vorsichtig sein.
Besonders ``-Ofast`` kann die Laufzeit verbessern, ist aber nicht immer unproblematisch.
Dieses Flag erlaubt dem Compiler aggressivere Optimierungen und kann unter anderem strenge Floating-Point-Regeln lockern.
Dadurch können Operationen umsortiert werden oder Annahmen ueber ``NaN``- und ``Inf``-Werte getroffen werden.
Für unsere Tsunami-Simulation bedeutet das: Auch wenn ``-Ofast`` schneller ist, muessen wir prüfen, ob die berechneten Wasserhoehen, Ankunftszeiten und Wellenausbreitungen weiterhin plausibel bleiben.
Deshalb vergleichen wir nicht nur die Laufzeit, sondern kontrollieren auch, ob die Simulationsergebnisse gegenüber ``-O2`` oder ``-O3`` sichtbar abweichen.

4. Optimierungsberichte
-----------------------

Compiler können Optimierungsberichte ausgeben.
Diese Berichte helfen zu verstehen, welche Schleifen optimiert oder vektorisiert wurden und welche Funktionen inline gesetzt wurden.
Für Clang haben wir im Build-Script die Option ``use_report`` ergänzt.
Wenn diese Option aktiviert ist und ``clang++`` verwendet wird, werden folgende Flags gesetzt:

.. code-block:: bash

    -Rpass=.*
    -Rpass-missed=.*
    -Rpass-analysis=.*

Der Build mit Clang-Report kann so gestartet werden:

.. code-block:: bash

    scons -c
    CXX=clang++ scons opt=O3 use_report=yes 2> clang_report.txt

Die Ausgabe wird in diesem Beispiel in ``clang_report.txt`` gespeichert.
Darin können wir nach den zeitintensiven Bereichen suchen, insbesondere nach ``WavePropagation2d::timeStep`` und ``fwave::netUpdates``.
Diese Teile sind für uns besonders interessant, weil die Zeitschritt-Schleife sehr oft ausgeführt wird und der f-wave Solver in jeder Zelle Updates berechnet.

Für GCC haben wir ebenfalls einfache Report-Flags ergänzt:

.. code-block:: bash

    -fopt-info-vec-optimized
    -fopt-info-vec-missed
    -fopt-info-inline-optimized
    -fopt-info-inline-missed

Damit kann auch fuer ``g++`` untersucht werden, ob Schleifen vektorisiert wurden und ob Funktionen inline gesetzt wurden.
Besonders wichtig sind dabei zwei Fragen:

* Kann der Compiler die grossen Schleifen in der Zeitschrittberechnung vektorisieren?
* Wird der f-wave Solver, insbesondere ``fwave::netUpdates``, in die Zeitschritt-Schleife hinein inlined?

Falls der Bericht zeigt, dass wichtige Schleifen nicht vektorisiert werden, können mögliche Ursachen Datenabhaengigkeiten, Funktionsaufrufe innerhalb der Schleife oder unklare Speicherzugriffe sein.
Falls ``fwave::netUpdates`` nicht inlined wird, kann das ebenfalls Laufzeit kosten, weil diese Funktion sehr häufig aufgerufen wird.
Die Optimierungsberichte geben uns damit Hinweise, an welchen Stellen weitere Performance-Verbesserungen sinnvoll sein koennten.


8.3 Instrumentalisierung und Performance Zähler
===============================================

1. VTune
--------

2. Verbesserung der Performance
-------------------------------
