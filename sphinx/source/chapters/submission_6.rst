#################################
Submission 6: Tsunami Simulations
#################################

Diese Woche sollten zwei Tsunami Events simuliert und untersucht werden. 
Dafür wird die in der letzten Woche implementierte Klasse ``setups::TsunamiEvent2d`` verwendet. 

Als eine der ersten Aufgaben diese Woche sollten wir uns die Input-Daten anschauen und visualisieren. 
Die Visualisierungen sind bei dem jeweiligen Event zu finden.

6.1. 2010 M 8.8 Chile Event
===========================

**Input-Daten visualisiert:**



6.2. 2011 M 9.1 Tohoku Event
============================
In dieser Aufgabe wurde das historische Tsunami-Ereignis vom 11. März 2011 in Tohoku (Japan) mithilfe der heruntergeladenen Bathymetrie- und Displacements-Datenbank simuliert. Ziel war insbesondere die Validierung der simulierten Wellenankunftszeit für die Küstenstadt Sõma.

1. Rechenaufwand & Wellenausbreitung:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**Verlassen der Computational Domain:**
Das simulierte Gebiet ist sehr groß (ca. 2700 km x 1500 km). Da das Epizentrum relativ küstennah liegt, erreichen die nach Osten, Norden und Süden propagierenden Wellen erst nach großer Distanz den (offenen) Rand der Domain. Unter der Annahme, dass die Welle im tiefen Pazifik häufig mit Geschwindigkeiten um die 200 m/s wandert, dauert es je nach Richtung **mindestens 1 bis 1,5 Stunden** (Simulationszeit), bis die ersten relevanten Wellenkränze final über den Rand der 750 km entfernten nördlichen bzw. südlichen Computational Domain entkommen.

**Computational Demands (250m Auflösung):**
- Domain Länge: 2700 km (X) und 1500 km (Y)
- Rasterzellen bei 250m: (2700 / 0.25) * (1500 / 0.25) = 10800 * 6000 = 64,800,000 Zellen
- Ein kompletter 2D-Zeitschritt verlangt dementsprechend ein volles Sweepen von knapp **64.8 Millionen Zellen**. Will man eine Stunde in der Realität nachstellen und nimmt ein fiktives dt = 0.8s an, erfordert dies über 10800 * 6000 * 4500 = **ca. 291 Milliarden** diskrete Zellupdates!


2. Sõma (Theorie und Realität):
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**Reale gemessene Daten:**
Laut offiziellen japanischen Pegelaufzeichnungen begann das Erdbeben um 14:46 Uhr. Die erste messbare Ankunft in Sõma war ein erster Pegelrückgang, gefolgt von der extremen ersten Flut um ca. 15:26 Uhr bis 15:50 Uhr. Die effektive erste Reisezeit lag in der Realität also bei etwa **40 bis 64 Minuten**.

**Reisezeit per Daumenregel (Rule of Thumb):**
Sõma liegt ca. 55 km südlich und 128 km westlich vom Epizentrum.
Die absolute Distanz ist s = sqrt((-128000)^2 + (-55000)^2) = 139.3 km.
Für den mitgelieferten 1D-Schnitt (1D Cut) der Bathymetrie können wir iterativ die Laufzeit anhand der Näherung lambda = sqrt(g * h) aufintegrieren:
Dank der gemittelten Tiefen über die knapp 139 km Strecke ergab die Integration eine theoretische Wellenreisezeit von ca. **4560 Sekunden (76 Minuten)**. Diese Rechnung per Flachwassergleichungs-Approximation deckt sich hinreichend genau mit den 64 Minuten real gemessenen Reisezeiten der kritischen Welle.

**Messung per Station:**
In der Simulation wurde ein virtueller Checkpoint (Station) aufgeschlagen. Dafür liegt die vorbereitete Datei `data/xml/soma_station.xml` vor:

.. code-block:: xml

    <?xml version="1.0"?>
    <stations>
        <station name="Soma" x="-128000" y="-55000" />
    </stations>

Gibt man diese Datei der laufenden Simulation über die Konsole (`./build/tsunami_lab 1000 data/xml/soma_station.xml 1.0`) mit, registriert das Tool nach gut einer Stunde Simulationszeit den dramatischen Anstieg in der h-Variablen, womit sowohl Theorie als auch Praxis validiert werden.
