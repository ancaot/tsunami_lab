import sys

content = """##################################################
5. Large Data Input and Output (NetCDF)
##################################################

5.1 Was bringt uns NetCDF?
==========================

Bisher haben wir unsere Simulationsergebnisse in reine CSV-Dateien geschrieben. Bei hochauflösenden Tsunami-Simulationen werden diese jedoch sehr schnell zu groß, das Schreiben dauert lange und die Visualisierung ist mühsam.

Die Lösung für große Datenmengen ist **NetCDF** (Network Common Data Form).

NetCDF ist ein bewährtes wissenschaftliches Binärformat, das:

- Daten extrem effizient speichert.
- "Selbstbeschreibend" ist, das heißt es speichert Metadaten, Dimensionen und Einheiten gleich mit.
- Ganze Simulationen über unendlich viele Zeitschritte komfortabel in *einer einzigen* Datei speichert statt in hunderten CSV-Files.
- Direkt und unkompliziert in Visualisierungs-Tools wie **ParaView** eingeladen werden kann.

5.2 Integration in unser Build-System (SCons)
=============================================

Bevor wir NetCDF nutzen können, müssen wir sicherstellen, dass unser Build-System (SCons) die benötigten C++ Bibliotheken (``zlib``, ``hdf5``, ``netcdf``) findet. 

Auf Debian-basierten Systemen kann man diese beispielsweise über das Terminal installieren:

.. code-block:: bash

    sudo add-apt-repository universe
    sudo apt-get update
    sudo apt-get install zlib1g-dev libhdf5-dev libnetcdf-dev

Anschließend passen wir unsere ``SConstruct`` an. Dank der SCons-Methoden erkennt unser Projekt automatisch, ob NetCDF auf dem Computer läuft, und bindet es dynamisch ein:

.. code-block:: python

    # SConstruct Anpassung für NetCDF
    if conf.CheckLibWithHeader('netcdf', 'netcdf.h', 'c'):
        env.Append(LIBS=['netcdf', 'hdf5_serial', 'z'])

5.3 Die Output-Schnittstelle (NetCdf::write)
============================================

Nun implementieren wir eine eigene Schnittstelle ``io::NetCdf``. 
Bevor eine Welle animiert werden kann, muss sie gespeichert werden. Dafür haben wir eine Methode eingebaut, die in jedem berechneten Fortschritt den aktuellen Wasserzustand in die ``.nc``-Datei updatet.

Hier ist ein grober Aufbau unserer Klasse aus der Datei ``NetCdf.h``:

.. code-block:: cpp

    namespace tsunami_lab {
      namespace io {
        class NetCdf;
      }
    }

    class tsunami_lab::io::NetCdf {
      public:
        // Schreibt den aktuellen Zeitschritt in die Datei
        void writeTimeStep( ... );
        
        // Liest Daten wie Bathymetrie aus einer Datei
        static int read( const char* filename,
                         const char* varname,
                         std::vector<t_real> &data );
    };

**Wichtige Besonderheiten bei der Implementierung:**

1. **Unlimitierte Zeitschritte (NC_UNLIMITED):** Die Zeit-Dimension ist in NetCDF als ``NC_UNLIMITED`` definiert. Das bedeutet, unsere Datei wächst mit jedem neuen Simulationsschritt dynamisch weiter, anstatt dass wir die Zeit-Größe fest vorgeben müssen.
2. **COARDS Konvention:** Die generierte NetCDF-Datei folgt strikt dem *COARDS*-Standard. Attribute und Einheiten wie ``units="meters"`` oder die Zeitangabe ``"seconds since..."`` werden explizit mit in die Datei geschrieben. ParaView liest das beim Importieren sofort aus und richtet die Zeitachse automatisch richtig ein!
3. **Ignorieren der Ghost-Cells:** Unser interner C++ Solver rechnet an den Rändern mit sogenannten "Ghost-Cells". Diese existieren physikalisch aber nicht. Um unsere Output-Datei sauber zu halten und Speicherplatz zu sparen, blendet unser NetCdf-Writer diese Ghost-Cells einfach beim Abspeichern aus.

5.4 Die Input-Schnittstelle (NetCdf::read)
==========================================

Tsunamis entstehen nicht auf perfekten, flachen Platten. Die Realität braucht topografische Daten für die Wassertiefe (Bathymetrie) und die Bodenverschiebung nach einem Erdbeben (Displacement). Solche hochauflösenden Karten liefert die Universität als NetCDF-Format.

Damit wir diese Simulationsbedingungen dynamisch laden können, haben wir die Funktion ``read`` gebaut. Sie nimmt den Dateinamen und die gewünschte Variable (z. B. "z" für den Höhenwert) und füllt die Daten in unsere C++-Vektoren (``std::vector``). 

.. code-block:: cpp

    // Verwendung zum Einlesen der echten Bathymetrie:
    std::vector<t_real> m_bathymetry_values;
    tsunami_lab::io::NetCdf::read("data/artificialtsunami_bathymetry_1000.nc",
                                  "z",
                                  m_bathymetry_values);

5.5 Wie nutzt man das in der Praxis?
====================================

**Dateien untersuchen mit ncdump:**

Will man schnell überprüfen, welche Daten, Dimensionen und Metainformationen eine NetCDF-Datei hat, ohne sie überhaupt zu öffnen, ist das Tool ``ncdump`` in der Konsole genial:

.. code-block:: bash

    # Zeigt alle Header-Informationen und verfügbare Variablen wie x, y, z
    ncdump -h data/artificialtsunami_bathymetry_1000.nc

**Visuelles Rendering mit ParaView:**

Dank der NetCDF-Struktur und den korrekten Einheiten ist die Visualisierung nun ein Kinderspiel:

1. Die exportierte Simulationsdatei (z.B. ``solution.nc``) per Drag-and-Drop in **ParaView** ziehen.
2. Im *Properties Panel* können wir aus den gespeicherten Layern z. B. *Height* (Wassertiefe) oder *ImpulseX* (Momentum) auswählen.
3. Danach drücken wir einfach auf den **Play-Button** am oberen Rand. Die Wasserverdrängung wird nun Frame für Frame animiert, und wir sehen den simulierten Tsunami über die abgebildete Küste brechen!
"""

with open("sphinx/source/chapters/submission_5.rst", "w", encoding="utf-8") as f:
    f.write(content)
