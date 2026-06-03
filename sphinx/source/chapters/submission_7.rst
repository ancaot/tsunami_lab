#############################################
Submission 7: Checkpointing and Coarse Output
#############################################

In dieser Woche wurden zwei Erweiterungen umgesetzt: Checkpointing für das
Fortsetzen einer abgebrochenen Simulation und Coarse Output zur Reduktion der
NetCDF-Ausgabegröße.

Die offizielle Aufgabenstellung beschreibt, dass ein Checkpoint alle
Informationen enthalten muss, die fuer einen Restart notwendig sind:
Gittergröße, Domain, Solver, Setup, Randbedingung, Bathymetrie, Wasserhöhe
und Impulse. Fuer den Coarse Output sollen jeweils ``k x k`` benachbarte Zellen
zu einer Ausgabezelle gemittelt werden.

7.1. Checkpointing
==================

Implementierung
---------------

Der NetCDF-Writer wurde in ``src/io/NetCdf/NetCdf.cpp`` erweitert. Die Methode
``createCheckPoint`` schreibt eine Datei nach
``outputs/checkpoints/checkpoint.nc``. Darin werden folgende Daten gespeichert:

* numerischer Solver, z.B. ``fwave`` oder ``roe``,
* verwendetes Szenario, z.B. ``tsunamievent2d``,
* Wave-Model, also ``1d`` oder ``2d``,
* Domain-Grösse und Domain-Start,
* Anzahl der Zellen in x- und y-Richtung,
* Endzeit der Simulation,
* Ausgabeformat und Dateinamen der Eingabedaten,
* Randbedingung,
* aktuelle Simulationszeit,
* ``h``, ``hu``, ``hv`` und ``b`` ohne Ghost Cells.

Beim Schreiben wird ``nc_sync`` verwendet, bevor die Datei geschlossen wird.
Dadurch werden NetCDF-Puffer auf die Festplatte geschrieben. Das ist für den
Crash-Test wichtig, weil ein Abbruch mit ``CTRL-C`` sonst noch Daten im
Speicher verlieren könnte.

Zusätzlich gibt es die Methode ``readCheckPoint``. Sie liest die gespeicherten
Meta-Daten und Felder wieder ein. Das Ergebnis wird an das neue Setup
``setups::CheckPoint`` weitergegeben.

CheckPoint-Setup
----------------

Das Setup liegt in ``src/setups/CheckPoint``. Es bekommt die aus dem
Checkpoint gelesenen Arrays und gibt sie wie ein normales Setup über die
Methoden

* ``getHeight``,
* ``getMomentumX``,
* ``getMomentumY``,
* ``getBathymetry``

zurück. Damit kann der bestehende Initialisierungscode wiederverwendet werden:
Der Solver merkt nicht, ob die Werte aus ``TsunamiEvent2d`` oder aus einem
Checkpoint kommen.

Automatischer Restart
---------------------

Im ``main.cpp`` wird beim Start geprüft, ob
``outputs/checkpoints/checkpoint.nc`` existiert. Wenn die Datei vorhanden ist,
werden die Simulationswerte aus dieser Datei gelesen und das Programm startet
automatisch mit ``setups::CheckPoint``. Wenn keine Checkpoint-Datei existiert,
nutzt das Programm die normale Initialisierung aus der Konfiguration.

Dadurch kann man eine Simulation mit denselben Parametern fortsetzen:

.. code-block:: powershell

    python -m SCons -Q netcdf=on `
      netcdf_include="C:\Program Files\netCDF 4.10.0\include" `
      netcdf_libdir="C:\Program Files\netCDF 4.10.0\lib"

    $env:PATH = "C:\Program Files\netCDF 4.10.0\bin;" + $env:PATH
    .\build\tsunami_lab.exe

Falls die Simulation abgebrochen wird, kann derselbe Befehl erneut ausgeführt
werden. Das Programm erkennt den Checkpoint und setzt die Simulation an der
gespeicherten Zeit fort.

Test
----

Das Checkpointing wird mit zwei Tests abgedeckt:

* ``NetCdfCheckpoint`` schreibt einen Checkpoint, liest ihn wieder ein und
  vergleicht alle gespeicherten Felder und Meta-Daten.
* ``CheckPointSetup`` prueft, ob ``setups::CheckPoint`` aus den geladenen
  Arrays wieder korrekte Werte fuer ``h``, ``hu``, ``hv`` und ``b`` liefert.

Der Testlauf war erfolgreich:

.. code-block:: text

    All tests passed (10310 assertions in 23 test cases)

7.2. Coarse Output
==================

Implementierung
---------------

Der NetCDF-Writer besitzt nun einen Coarse-Faktor ``k``. Dieser Wert wird beim
Erzeugen des Writers uebergeben und kann in der Konfiguration als
``coarse_factor`` gesetzt werden:

.. code-block:: json

    {
      "output_format": "netcdf",
      "coarse_factor": 20
    }

Fuer jedes Ausgabefeld wird aus dem feinen Rechengitter ein grobes Ausgabegitter
berechnet. Dabei werden jeweils bis zu ``k x k`` benachbarte Zellen gemittelt.
Am Rand wird nicht abgeschnitten: Wenn die Zellzahl nicht durch ``k`` teilbar
ist, wird die letzte grobe Zelle aus den noch vorhandenen feinen Zellen
berechnet.

Damit gilt:

``out_nx = ceil(nx / k)``

``out_ny = ceil(ny / k)``

Beispiel:

* ``nx = 2700``, ``ny = 1500``, ``k = 10``
* Ausgabe: ``ceil(2700 / 10) x ceil(1500 / 10)``
* Ausgabe: ``270 x 150`` Zellen pro Zeitschritt

Dadurch bleibt die Simulation intern fein, aber die NetCDF-Datei wird deutlich
kleiner.

Tohoku mit 50m Zellweite
------------------------

Fuer die Aufgabe soll das Tohoku-Ereignis mit derselben Domain wie in Kapitel
6.2 simuliert werden. Die Domain ist

``2700000 m x 1500000 m``.

Bei einer Zellweite von ``50 m`` ergibt sich:

``nx = 2700000 / 50 = 54000``

``ny = 1500000 / 50 = 30000``

Das sind

``54000 * 30000 = 1,620,000,000`` Zellen.

Diese Simulation ist lokal sehr gross. Allein ein einzelnes Feld mit
``float``-Werten benoetigt ungefaehr

``1.62e9 * 4 Byte = 6.48 GB``.

Da der 2D-Solver mehrere Felder und Ghost Cells verwaltet, ist der echte
Speicherbedarf deutlich hoeher. Deshalb wurde die komplette 50m-Simulation
nicht lokal bis zum Ende ausgeführt. Fuer die Aufgabe reicht es, die ersten
Zeitschritte zu starten oder einen kleineren Datensatz zu verwenden.

Beispiel-Konfiguration fuer die 50m-Variante:

.. code-block:: json

    {
      "numerical_solver": "fwave",
      "scenario": "tsunamievent2d",
      "wave_model": "2d",
      "domain_size_x": 2700000,
      "domain_size_y": 1500000,
      "cells_x": 54000,
      "cells_y": 30000,
      "coarse_factor": 20,
      "origin_x": -200000,
      "origin_y": -750000,
      "simulation_end_time": 200,
      "output_format": "netcdf",
      "output_name": "tohoku_50m_coarse_k20.nc",
      "reflective_boundary": false
    }

Mit ``k = 20`` wird aus der internen 50m-Simulation eine Ausgabe mit
``1000 m`` effektiver Ausgabezellweite. Das Ausgabegitter ist dann

``ceil(54000 / 20) x ceil(30000 / 20) = 2700 x 1500``.

Das ist immer noch groß, aber wesentlich kleiner als die volle 50m-Ausgabe.

Visualisierung
--------------

Die grobe NetCDF-Datei kann in ParaView direkt geöffnet werden. Für die
Visualisierung wurden die Variablen

* ``h`` für Wasserhöhe,
* ``hu`` und ``hv`` für Impulse,
* ``b`` für Bathymetrie

verwendet. Der Vorteil des Coarse Outputs ist, dass ParaView nicht die komplette
50m-Auflösung laden muss. Dadurch lässt sich die Ausbreitung der Welle trotz
feiner Simulation deutlich einfacher betrachten.

Zusammenfassung
===============

Checkpointing und Coarse Output sind nun miteinander kombinierbar:

* Der Solver schreibt regelmaessig einen Restart-Zustand nach
  ``outputs/checkpoints/checkpoint.nc``.
* Bei einem erneuten Start wird automatisch aus diesem Checkpoint fortgesetzt.
* Die NetCDF-Ausgabe kann durch ``coarse_factor`` verkleinert werden.
* ``nc_sync`` reduziert das Risiko, dass ein Abbruch kurz nach dem Schreiben
  einen unvollständigen Checkpoint hinterlässt.

Damit ist die Simulation robuster gegen Unterbrechungen und erzeugt gleichzeitig
handlichere Ausgabedateien für grosse Szenarien wie Tohoku.
