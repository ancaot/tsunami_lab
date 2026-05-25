###########################################
Submission 5: Large Data I/O (Input/Output)
###########################################

Was bringt diese Woche?
=======================

Das Schreiben und Lesen von reinen CSV-Dateien wird bei hochauflösenden Tsunami-Simulationen sehr schnell zu groß, langsam und mühsam.
Die Lösung für **große Datenmengen** ist **NetCDF** (Network Common Data Form).
Es ist ein Binärformat, das:
- Daten extrem effizient speichert.
- "Selbstbeschreibend" ist (speichert Metadaten, Einheiten, Skalierungen).
- Ganze Simulationen (über unendlich viele Zeitschritte) in *einer einzigen* Datei speichert statt in 1000 CSV-Files.
- Direkt in **ParaView** animiert werden kann.


Ich habe mich diese Woche um die **Schnittstelle für NetCDF** gekümmert. Konkret habe ich folgende Punkte umgesetzt:


5.1. NetCDF Output


1. Integration in unser Build-System (SCons)
-----------------------------------------------
NetCDF bringt Abhängigkeiten wie `zlib` und `HDF5` mit. 
Damit unser Simulator NetCDF-Dateien verstehen kann, habe ich die Build-Konfiguration in unserer ``SConstruct`` angepasst:
- Dank der eingebauten SCons-Methoden ``CheckLib`` und ``CheckLibWithHeader`` erkennt unser Projekt automatisch, ob NetCDF und die benötigten Bibliotheken (``zlib``, ``hdf5``) auf dem System verfügbar sind.
- Unser Code wird nur gegen NetCDF gelinkt, wenn es auch erfolgreich gefunden wird.

2. Die Output-Schnittstelle (NetCdf::writeTimeStep)
------------------------------------------------------
Bevor eine Karte gezeichnet werden kann, muss sie gespeichert werden. Dafür habe ich in unserer neuen Klasse ``io::NetCdf`` eine Funktion implementiert, die in jedem Zeitschritt den **aktuellen Zustand des Wassers in eine .nc-Datei schreibt**.
- **Besonderheit Zeitschritt:** Die Zeit-Dimension ist als ``NC_UNLIMITED`` verknüpft, wodurch die Datei mit jedem gespeicherten Zeitschritt in der Länge wachsen kann, ohne vorher eine Größe festzulegen.
- **COARDS Konvention:** Die NetCDF-Datei folgt dem COARDS Standard. Parameter wie die Einheit (``units="m"``) oder die Zeit ("seconds since the earthquake event") werden explizit mitgeschrieben. Dadurch triggern wir automatisch die korrekte Zeit-Domäne im Visualisierungs-Tool (ParaView).
- **Ausschluss von Ghost-Cells:** Ghost-Cells sind nur für die Berechnungen an den Rändern nützlich, physikalisch existieren sie aber nicht. Um Platz zu sparen und saubere Bilder zu erhalten, ignoriert unsere NetCDF-Speicherfunktion diese Randzellen standardmässig.



5.2.2
--------------------------------------------
Ein Tsunami entsteht natürlich nicht aus dem Nichts – wir benötigen reale Daten für die Wassertiefe (Bathymetrie) und die Bodenverschiebung (Displacement) nach einem Erdbeben. Solche Daten stellt die Universität im NetCDF-Format zur Verfügung.
- Ich habe unsere ``io::NetCdf`` Klasse erweitert, sodass nun Variablen (z. B. "x", "y", "z") direkt aus externen ``.nc``-Dateien ausgelesen werden können.
- Die eingelesenen Daten werden in dynamische Vektoren (``std::vector``) geladen und stehen dem Simulator anschließend für die Berechnungen zur Verfügung.
- Um das Lesen und die Struktur sicher testen zu können, nutzen wir kleine Dummy-Dateien, die wir mit Tools wie ``ncgen`` erzeugen, oder überprüfen die Dateien in der Konsole über ``ncdump -h <file.nc>``.

Wie nutzt man das in der Praxis?
====================================

**Datei inspizieren:**

Wenn man prüfen will, welche Daten in einer NetCDF-Datei stecken, kann man auf der Konsole den ncdump-Befehl nutzen:

.. code-block:: bash

    # Zeigt Header-Informationen und verfügbare Variablen
    ncdump -h artificialtsunami_bathymetry_1000.nc

**Visualisierung mit ParaView:**

Dank der Einhaltung der COARDS-Regeln und der Angabe des "since"-Keywords versteht ParaView die Datei sofort:
1. ``solution.nc`` in ParaView öffnen.
2. Im *Properties Panel* Parameter (z.B. *Height*) anwenden.
3. Den **Play-Button** drücken und die Ausbreitung des Tsunamis wird sofort flüssig als Karte animiert dargestellt.


5.2 NetCDF Input
----------------

Ich habe mich diese Woche um die Aufgaben 5.2.1, 5.2.3 und 5.2.4 gekümmert.

Auf Grund von Problemen bei der Kompilierung meinerseits, konnte ich leider nicht die neuen Setups vollständig testen.
Deswegen gibt es keine konkrete Antwort zu Aufgabe 5.2.4. und die anderen beiden Aufgaben konnten nur teilweise gelöst werden.


1. Das künstliche zwei-dimensionale Tsunami-Setups
--------------------------------------------------

**5.2.1** beschäftigt sich mit dem Hard-Coding eines künstlichen Tsunami-Setups.
Dafür wurde eine neue Klassen namens ``setups::ArtificialTsunami2d`` implementiert und getestet.

Bei dieser Implementierung wird noch keine Einlesung von Dateien erwartet.
Die Wassertiefe und die Bodenverschiebung sind vorgegeben, beziehungsweise mit einer bestimmten Formel zu berechnen.
Die Wassertiefe liegt in diesem Setup bei -100m.

Für die Berechnung der Bodenverschiebung wurden drei neue Methoden eingeführt.

Die erste Methode namens ``getDisplacement(i_x, i_y)`` berechnet die Bodenverschiebung :math:`d(x,y)=5*f(x)*g(y)`.
Dazu werden in dieser Methode, die zwei nachfolgenden Methoden aufgerufen.

Die zweite Methode berechnet die Formel für :math:`f(x)` und die dritte berechnet die Formel für :math:`g(y)`.

**Test Ergebnisse**:
    #. Die Wassertiefe ist, wie schon geschrieben überall -100m. Die Wasserhöhe wurde auf 10m gesetzt.
    #. nicht verfügbar
    

2. Das zwei-dimensionale Tsunami Event
--------------------------------------

Die Aufgabe **5.2.3** integriert eine neue Klassen ``setups::TsunamiEvent2d`` in unser Programm.
Um die Daten aus den NetCDF Dateien effizient auf unser Gitter zu projizieren, nutzt die Klasse nun eine **Nearest Neighbour Interpolation** für die Zuweisung der passenden Gitterwerte.

Zur Ausführung einer Simulation: Die Startparameter in der `main.cpp` können nun so konfiguriert werden, dass das neue Setup simuliert und die Daten ausgegeben werden. Erste Testläufe wurden erfolgreich über die Einbindung im `SConscript` compiliert und durchlaufen. 
Im Gegensatz zu Aufgabe **5.2.1** verwendet diese Klasse Datein Input für die Wassertiefe und Bodenverschiebung.
Dazu wird die Methode aus **5.2.2** verwendet um die NetCDF-Dateien zu lesen.

Die Höhe, die Momenta und die Wassertiefe werden wie in der Aufgabenstellung initalisiert.
Dazu wurden die jeweiligen Getter-Methoden angepasst.

Für die Höhe und die Wassertiefe werden die Dateien Inputs benötigt, wodurch jeweils die nötigen Dateien in der Methode aufgerufen werden. 
Dazu kommt, dass für das lesen der Dateien die Resolution benötigt wird, um den Bedingungen der Aufgabenstellung nachzugehen. 
Also für die Höhenbestimmung, wird die Datei mit den Informationen über die Wassertiefe aufgerufen, um damit die gegeben Ansprüche zu erfüllen. 
Die Bestimmung der Wassertiefe braucht zum einen die gelesenen Wassertiefen und dazu die gelesenen Bodenverschiebungsdaten.

Um an die gewünschte Resolution und die Simulationszeit heranzukommen, wird bei der Ausführung der ``main.cpp`` Datei neben der Zellengröße nach beidem gefragt.



3. Vergleich von der künslichen Simulation und dem Tsunami Event
----------------------------------------------------------------

Die Aufgabe **5.2.4** erwartet einen Korrektheitscheck für die Dateien-Input-Klasse, indem diese mit der künstlichen Tsunami Simulation verglichen wird. 
Dafür soll die in der Aufgabenstellung verlinkten Input-Daten verwendet werden.

**Test Ergebnisse**: nicht verfügbar
Assignment 6: Tsunami Simulations
=================================

Aufgabe 6.2: 2011 M 9.1 Tohoku Event
------------------------------------
In dieser Aufgabe wurde das historische Tsunami-Ereignis vom 11. März 2011 in Tohoku (Japan) mithilfe der heruntergeladenen Bathymetrie- und Displacements-Datenbank simuliert. Ziel war insbesondere die Validierung der simulierten Wellenankunftszeit für die Küstenstadt Sõma.

1. Rechenaufwand & Wellenausbreitung:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**Verlassen der Computational Domain:**
Das simulierte Gebiet ist sehr groß (ca. 2700 km x 1500 km). Da das Epizentrum relativ küstennah liegt, erreichen die nach Osten, Norden und Süden propagierenden Wellen erst nach großer Distanz den (offenen) Rand der Domain. Unter der Annahme, dass die Welle im tiefen Pazifik häufig mit Geschwindigkeiten um die :math:200\,m/s wandert, dauert es je nach Richtung **mindestens 1 bis 1,5 Stunden** (Simulationszeit), bis die ersten relevanten Wellenkränze final über den Rand der 750 km entfernten nördlichen bzw. südlichen Computational Domain entkommen.

**Computational Demands (250m Auflösung):**
- Domain Länge: 2700 km (X) und 1500 km (Y)
- Rasterzellen bei 250m: :math:(2700 / 0.25) \times (1500 / 0.25) = 10800 \times 6000 = 64,800,000 Zellen
- Ein kompletter 2D-Zeitschritt verlangt dementsprechend ein volles Sweepen von knapp **64.8 Millionen Zellen**. Will man eine Stunde in der Realität nachstellen und nimmt ein fiktives :math:dt \approx 0.8s an, erfordert dies über :math:10800 \times 6000 \times 4500 = **ca. 291 Milliarden** diskrete Zellupdates!


2. Sõma (Theorie und Realität):
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**Reale gemessene Daten:**
Laut offiziellen japanischen Pegelaufzeichnungen begann das Erdbeben um 14:46 Uhr. Die erste messbare Ankunft in Sõma war ein erster Pegelrückgang, gefolgt von der extremen ersten Flut um ca. 15:26 Uhr bis 15:50 Uhr. Die effektive erste Reisezeit lag in der Realität also bei etwa **40 bis 64 Minuten**.

**Reisezeit per Daumenregel (Rule of Thumb):**
Sõma liegt ca. 55 km südlich und 128 km westlich vom Epizentrum.
Die absolute Distanz ist :math:s \approx \sqrt{(-128000)^2 + (-55000)^2} \approx 139.3 \text{ km}.
Für den mitgelieferten 1D-Schnitt (1D Cut) der Bathymetrie können wir iterativ die Laufzeit anhand der Näherung :math:\lambda \approx \sqrt{g \cdot h} aufintegrieren:
Dank der gemittelten Tiefen über die knapp 139 km Strecke ergab die Integration eine theoretische Wellenreisezeit von ca. **4560 Sekunden (76 Minuten)**. Diese Rechnung per Flachwassergleichungs-Approximation deckt sich hinreichend genau mit den 64 Minuten real gemessenen Reisezeiten der kritischen Welle.

**Messung per Station:**
In der Simulation wurde ein virtueller Checkpoint (Station) aufgeschlagen. Dafür liegt die vorbereitete Datei `data/xml/soma_station.xml` vor:

.. code-block:: xml

    <?xml version="1.0"?>
    <stations>
        <station name="Soma" x="-128000" y="-55000" />
    </stations>

Gibt man diese Datei der laufenden Simulation über die Konsole (`./build/tsunami_lab 1000 data/xml/soma_station.xml 1.0`) mit, registriert das Tool nach gut einer Stunde Simulationszeit den dramatischen Anstieg in der :math:h-Variablen, womit sowohl Theorie als auch Praxis validiert werden.
