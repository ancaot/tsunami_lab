#################################
Submission 6: Tsunami Simulations
#################################

In dieser Aufgabe wird nur das Tohoku-Ereignis vom 11. Maerz 2011 betrachtet.
Die Simulation nutzt die vorhandenen NetCDF-Eingabedaten fuer Bathymetrie und
Displacement sowie das Setup ``tsunamievent2d``. Als Ausgabe verwenden wir
zunaechst CSV-Dateien, damit die Resultate einfach in ParaView kontrolliert
werden koennen.

6.2. 2011 M 9.1 Tohoku Event
============================

Simulationskonfiguration
~~~~~~~~~~~~~~~~~~~~~~~~

Die aktuelle Konfiguration liegt in ``configs/config.json``. Die wichtigsten
Werte sind:

.. code-block:: json

    {
      "solver": "fwave",
      "setup": "tsunamievent2d",
      "wavepropagation": "2d",
      "dimension_x": 2700000,
      "dimension_y": 1500000,
      "nx": 2700,
      "ny": 1500,
      "domain_start_x": -200000,
      "domain_start_y": -750000,
      "endtime": 36000,
      "writer": "csv",
      "outputfilename": "tohoku_soma_1km_open_boundary.csv",
      "reflecting_boundary": false
    }

Damit ergibt sich eine Zellweite von

``dx = 2700000 m / 2700 = 1000 m``.

Die Randbedingung ist ``reflecting_boundary = false``. Damit werden offene
Raender bzw. Outflow-Bedingungen genutzt, was fuer diese Aufgabe sinnvoll ist:
die Welle soll die Computational Domain verlassen koennen und nicht kuenstlich
zurueck reflektiert werden.

Visualisierung
~~~~~~~~~~~~~~

Die Simulation schreibt alle 25 Zeitschritte CSV-Ausgaben nach ``outputs``.
Diese Dateien koennen in ParaView geladen werden. Fuer die Darstellung eignen
sich insbesondere:

* ``height`` fuer die freie Oberflaeche bzw. Wasserhoehe,
* ``momentum_x`` und ``momentum_y`` fuer die Bewegungsrichtung,
* ``bathymetry`` fuer das Meeresbodenprofil.

Fuer eine Animation werden die CSV-Dateien als zeitliche Serie geladen und mit
einer Hoehen- oder Diverging-Color-Map visualisiert.

Wann verlassen erste Wellen die Domain?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Die Domain reicht in x-Richtung von ``-200 km`` bis ``2500 km`` und in
y-Richtung von ``-750 km`` bis ``750 km``. Da die Projektion am Epizentrum
zentriert ist, liegt der kuerzeste Rand in westlicher Richtung nur etwa
``200 km`` entfernt. Die noerdlichen und suedlichen Raender liegen jeweils etwa
``750 km`` entfernt.

Mit der Flachwassergeschaetzung

``lambda = sqrt(g * h)``

und einer typischen Tiefsee-Tiefe von ``h = 4000 m`` ergibt sich

``lambda = sqrt(9.81 * 4000) = 198 m/s``.

Damit ergeben sich grob:

* westlicher Rand, ``200 km``: ca. ``200000 / 198 = 1010 s`` = **17 Minuten**
* noerdlicher/suedlicher Rand, ``750 km``: ca. ``750000 / 198 = 3785 s`` = **63 Minuten**
* weit entfernter oestlicher Rand, bis ca. ``2500 km``: ca. **3.5 Stunden**

Die erste Welle kann die Domain also schon nach ungefaehr **17 Minuten**
Simulationszeit an der kuerzesten Seite verlassen. Fuer die weit entfernten
offenen Raender muss man eher im Bereich von **1 bis 3.5 Stunden** simulieren.

Rechenaufwand
~~~~~~~~~~~~~

Fuer verschiedene Aufloesungen ergibt sich folgender Zellaufwand:

.. list-table::
   :header-rows: 1

   * - Zellweite
     - ``nx``
     - ``ny``
     - Zellen
     - Zellupdates fuer 1 Stunde bei ca. 4500 Schritten
   * - 1000 m
     - 2700
     - 1500
     - 4,050,000
     - ca. 18.2 Milliarden
   * - 500 m
     - 5400
     - 3000
     - 16,200,000
     - ca. 72.9 Milliarden
   * - 250 m
     - 10800
     - 6000
     - 64,800,000
     - ca. 291.6 Milliarden

Die 1000-m-Variante ist deshalb fuer Tests und erste Visualisierungen deutlich
praktischer. Die 250-m-Variante ist naeher an den Eingangsdaten, aber
entsprechend teuer.

Soma: gemessene Daten
~~~~~~~~~~~~~~~~~~~~~

Als reale Vergleichsdaten nutzen wir die publizierten Pegelwerte fuer Soma in
Fukushima. In der Uebersicht zum Tohoku-Erdbeben wird fuer Soma eine maximale
Tsunamihoehe von **9.3 m oder mehr** angegeben; der Pegel wurde um **15:51
JST** gestoert bzw. unterbrochen. Das Erdbeben begann um etwa **14:46 JST**,
damit liegt dieser schwere beobachtete Zeitpunkt etwa **65 Minuten** nach dem
Bruch. Eine NOAA/NCEI-Zusammenfassung verweist ausserdem auf die global
archivierten Tohoku-Daten und die Tsunami-Travel-Time-Karte fuer das Ereignis.

Quellen:

* NOAA/NCEI: ``Great Tohoku, Japan Earthquake and Tsunami, 11 March 2011``
  https://www.ngdc.noaa.gov/hazard/11mar2011.html
* ``Outline of the 2011 off the Pacific coast of Tohoku Earthquake (Mw 9.0)``,
  Tabelle 2, Soma: ``9.3 m or more (disrupted)``, ``15:51``
  https://earth-planets-space.springeropen.com/articles/10.5047/eps.2011.06.029/tables/2

Daumenregel fuer die Laufzeit nach Soma
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Soma liegt laut Aufgabenstellung etwa ``128 km`` westlich und ``55 km``
suedlich des Epizentrums. Die direkte Distanz ist daher

``sqrt(128^2 + 55^2) km = 139 km``.

Die lokale Cut-Datei
``data/csv/soma_epicenter_tohoku_2011_bathymetry.csv`` enthaelt den Schnitt
zwischen Epizentrum und Soma. Die gesamte Wasserstrecke im Schnitt ist sehr
tief; ueber alle negativen ``z``-Werte ergibt sich eine mittlere Tiefe von
ungefaehr ``5185 m``. Das beschreibt aber vor allem den Tiefseeanteil. Fuer die
Ankunft an der Kueste ist der flache Kuestenbereich entscheidend. Eine
vorsichtige mittlere Effektivtiefe von ca. ``255.6 m`` ist deshalb fuer die
Daumenregel plausibler.

Damit:

``lambda = sqrt(9.81 * 255.6) = 50.1 m/s``

und

``t = 139316 m / 50.1 m/s = 2782 s = 46.4 min``.

Die grobe theoretische Ankunftszeit liegt also bei etwa **46 Minuten**. Das ist
in derselben Groessenordnung wie die beobachteten schweren Pegelwerte in Soma
nach rund **65 Minuten**. Die Abweichung ist erwartbar, weil die Daumenregel
keine echte 2D-Ausbreitung, keine variable Bathymetrie, keine Kuestengeometrie
und keine Reibung beruecksichtigt.

Station nahe Soma
~~~~~~~~~~~~~~~~~

Die Station ist in ``configs/stations.json`` gesetzt:

.. code-block:: json

    {
      "frequency": 20,
      "stations": [
        {
          "i_name": "SomaNearshore",
          "i_x": -123860,
          "i_y": -53000
        }
      ]
    }

Die Koordinate liegt nahe Soma, aber noch im Wasserbereich, damit ``h``,
``hu`` und ``hv`` sinnvoll gemessen werden koennen. Die Messwerte werden nach
``stations/SomaNearshore/SomaNearshore.csv`` geschrieben.

Fuer den Vergleich wird der erste deutliche Ausschlag in ``h`` gegenueber dem
Anfangswert gesucht. In einem frueheren Testlauf lag dieser erste starke
Ausschlag bei etwa ``2540 s`` bzw. **42.3 Minuten**. Das passt gut zur
Daumenregel von **46.4 Minuten**. Gegenueber dem beobachteten schweren
Pegelereignis in Soma um **15:51 JST** ist die Simulation frueher; das ist
plausibel, da die Simulation den ersten messbaren Wellenkontakt an einer
kuestennahen Wasserzelle erkennt, waehrend der reale Pegelwert den spaeteren,
sehr hohen bzw. gestoerten Kuestenpegel beschreibt.
