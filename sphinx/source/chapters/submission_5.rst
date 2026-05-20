########################################
Submission 5: Large Data I/O (Input/Output)
########################################

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
