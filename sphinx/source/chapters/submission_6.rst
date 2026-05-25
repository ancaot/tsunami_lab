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
FÃ¼r die zweite Fragestellung (Wellenreisezeit per ``lambda = sqrt(g * h)``) nutzt man die Werte aus dem ``data/csv/Bathymetry.csv`` Cut. 
Wie auf der National Centers for Environmental Information Datenbank zu entnehmen, war ein gemessener Reisezeitpunkt knapp 9 Minuten ab Bruch bzw maximal bis zu einigen wenigen Dutzend Minuten bis ins KÃ¼stenvorfeld.  
ZÃ¤hlt man die Tiefen in der Datei fÃ¼r den relevanten Wasserbereich zusammen, ergeben die Schnitttiefe einen approximativen Mean von ca. -255.6 m.
Nutzen wir diesen Durchschnitt fÃ¼r den Wellenspeed: `lambda = sqrt(9.81 * 255.6) = ca. 50 m/s`. Bei 139 km Luftlinie schlÃ¤gt die reine Laufzeit-Theroie also bei knapp **46 Minuten** ein (was als realer Threshold oft angegeben wird bevor die gewaltigen Nachbeben die KÃ¼ste treffen).

3. SÃµma Station Messung:
~~~~~~~~~~~~~~~~~~~~~~~~~
Nachdem wir theoretische 46 Minuten hergeleitet haben, fÃ¼gen wir nun eine virtuelle Wasserstands-Station relativ kÃ¼stennah vor SÃµma im Gitter hinzu.
Wir wÃ¤hlten dazu ungefÃ¤hr den Punkt ``-123860`` / ``-53000``, den ersten sicheren Meeres-Koordinatenwert, da wir dort (negative Bathymetrie) gut WasserhÃ¶hen ablesen kÃ¶nnen:

.. code-block:: xml

    <?xml version="1.0"?>
    <stations>
        <station name="SomaStation" x="-123860" y="-53000" />
    </stations>

Mit einem Simulationsdurchlauf Ã¼ber mehrere Stunden registriert die Station nach etwa 2540 Sekunden (~42,3 Minuten) die massive Ankunft der ersten Tsunamiwelle in unseren CSV-Logs.
Dieser gemessene Arrival liegt mit knapp 42,3 Minuten rund 3,87 Minuten vor unserer eignen Daumenrechnung (46,2 Minuten). Diese Differenz wird maÃŸgeblich durch die Abflachung / grobe Approximation der Bathymetrie-Liste und das Gitter bedingt sein. Zieht man man zusÃ¤tzlich die InitialhÃ¶he der ersten Welle auf dem Ozean ab, entsprÃ¤che die Amplitude ca. 7,7 Meter Tsunami-Runup - was dem Messwert von offiziellen 9,3 m recht nah kommt!

