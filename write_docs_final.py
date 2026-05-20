import sys

content = """##################################################
Submission 5: Large Data I/O (Input/Output)
##################################################

Was bringt diese Woche?
=======================

Das Schreiben und Lesen von reinen CSV-Dateien wird bei hochauflösenden Tsunami-Simulationen sehr schnell zu groß, langsam und mühsam.
Die Lösung für **große Datenmengen** ist **NetCDF** (Network Common Data Form).
Es ist ein Binärformat, das:

- Daten extrem effizient speichert.
- "Selbstbeschreibend" ist (speichert Metadaten, Einheiten, Skalierungen).
- Ganze Simulationen (über unendlich viele Zeitschritte) in *einer einzigen* Datei speichert statt in 1000 CSV-Files.
- Direkt in **ParaView** animiert werden kann.

Mein Teil der Aufgabe (Zusammenfassung)
=======================================

1. Installation und SCons-Integration
-------------------------------------
NetCDF bringt Abhängigkeiten wie `zlib` und `HDF5` mit. 
Damit unser Simulator NetCDF-Dateien verstehen und das Build-System die Dateien mit unserem Solver teilen kann, habe ich die Build-Konfiguration in unserer ``SConstruct`` angepasst:

- Dank der eingebauten SCons-Methoden ``CheckLib`` und ``CheckLibWithHeader`` erkennt unser Projekt automatisch, ob NetCDF und die benötigten Bibliotheken (``zlib``, ``hdf5``) auf dem System verfügbar sind.
- Unser Code wird nur gegen NetCDF gelinkt, wenn es auch erfolgreich gefunden wird.

2. Die Output-Schnittstelle (io::NetCdf)
----------------------------------------
Ich habe die Klasse ``io::NetCdf`` implementiert, die als unsere feste Schnittstelle zu den NetCDF-Dateien fungiert. Die Kernfunktion hierin speichert für jeden aktuellen Zeitschritt den Zustand in eine `.nc`-Datei.
Dabei habe ich speziell auf die Einhaltung der Vorgaben geachtet, um ein einfaches Postprocessing in ParaView zu garantieren:

- **Ausschluss von Ghost-Cells:** Berechnete Werte in den Ghost-Cells (Randzellen) werden standardmäßig NICHT mit in die Datei geschrieben, da sie nur für die interne Berechnung nützlich sind.
- **COARDS Konvention & "since"-Keyword:** Die Datei folgt dem COARDS Standard. Ein wichtiges Detail ist die Zeit-Attributierung über das Keyword *"since"* (z. B. *"seconds since the earthquake event"*). Genau dieses Keyword triggert beim Import in ParaView direkt die korrekte Zeit-Domäne.
- **Unlimitierte Dimensionen:** NetCDF unterstützt unlimitierte Dimensionen, was extrem hilfreich ist, um die Zeit-Dimension abzubilden (``NC_UNLIMITED``). Dadurch kann die Datei mit jedem neuen Simulationsschritt in der Länge wachsen.

3. Die Input-Schnittstelle (NetCdf::read)
-----------------------------------------
Zuletzt habe ich unsere Klasse ``io::NetCdf`` erweitert, um auch das Einlesen von NetCDF-Dateien zu unterstützen. 
Das ist wichtig, da wir reale topografische Daten für Wassertiefe und Bodenverschiebung im NetCDF-Format bereitgestellt bekommen.
    
**Hilfswerkzeuge für Tests und Praxis:**

- Um die implementierte ``read``-Funktion über Unit-Tests abzusichern, nutze ich das Tool **``ncgen``**, um unkompliziert kleine NetCDF-Dateien zu generieren.
- Wenn man schnell in der Konsole prüfen möchte, welche Variablen und Dimensionen in einer NetCDF-Datei existieren (z. B. in den Uni-Dateien), eignet sich am besten der Befehl: **``ncdump -h <file.nc>``**.
"""

with open("sphinx/source/chapters/submission_5.rst", "w", encoding="utf-8") as f:
    f.write(content)
