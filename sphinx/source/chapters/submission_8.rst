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

*Vergleichstabelle von persönlichen PC, interaktiven und batch Jobs*

.. list-table::
    :header-rows: 1

    * - Information
      - Persönlicher PC
      - Interaktiver Job
      - Batch Job
    * - cells
      - 771 x 429
      - 
      - 
    * - cell width
      - 3501.95 m
      - 
      - 
    * - time step
      - 5.72301 seconds
      - 
      - 
    * - steps
      - 630
      - 
      - 
    * - Duration of time step loop
      - 5 s, 726 ms, 966 µs, 700 ns
      - 
      - 
    * - Duration of programm
      - 7 s, 300 ms, 386 µs, 0 ns
      - 
      - 
     
Hier nochmal die Visualisierung von der berechnenten Simulation:

.. raw:: html

   <video src="../_static/tohoku_3500_personalPC.mp4" controls style="width: 72%; max-width: 760px; display: block; margin: 1rem auto;"></video>

Visualisierung der Daten berechnet mit persönlichen PC.


8.2 Compiler
============

1. Generische Compiler
----------------------

2. GNU vs Clang
---------------

3. Optimierungs-Switches
------------------------

4. Optimierungsberichte
-----------------------


8.3 Instrumentalisierung und Performance Zähler
===============================================

1. VTune
--------

2. Verbesserung der Performance
-------------------------------