###################################
Submission 5: Large Data I/O
###################################

Was bringt das?
===============

Das Problem: CSV-Dateien sind groß, langsam und schwer zu visualisieren.

**Die Lösung:** NetCDF (Network Common Data Form) – ein Binärformat, das:
- kompakter als CSV sein kann (Speicher sparen)
- schneller geladen werden kann (weniger Wartezeit)
- **ParaView versteht** (professionelle Visualisierung nutzen)
- Zeitschritte speichert (ganze Simulationen in einer Datei)

**Was machen wir?**

- **Aufgabe 1:** Simulator schreibt ``solution.nc`` statt CSV
- **Aufgabe 2:** Wir können die Datei wieder laden und Ergebnisse validieren

Schritt-für-Schritt: Was passiert?
==================================

1. **Simulator lädt die io::NetCdf Klasse** → erzeugt ``solution.nc`` Datei
2. **Jeden Zeitschritt** schreibt der Simulator Höhe, Impuls etc. rein
3. **Nach der Simulation** kann man die Datei laden und anschauen oder mit ParaView visualisieren

.. code-block:: 

    main.cpp            →  NetCdf Writer   →  solution.nc  →  ParaView (GUI)
    (Simulator)            (Aufgabe 1)          (Datei)         (Bild anschauen)
                                    ↓
                          NetCdf Reader
                          (Aufgabe 2)
                             ↓
                        Tests, Validierung


So baust du es
==============

**Mit NetCDF (empfohlen):**

.. code-block:: bash

    python -m SCons -Q netcdf=auto

Das Buildscript erkennt automatisch, ob NetCDF installiert ist.

**Ohne NetCDF (falls nicht installiert):**

.. code-block:: bash

    python -m SCons -Q netcdf=off

Der Code lädt dann normal, aber schreibt nicht in NetCDF.

Die io::NetCdf Klasse
====================

**Zum Schreiben:**

.. code-block:: cpp

    // Neue Datei erzeugen
    io::NetCdf writer("solution.nc", dxy, nx, ny, 
                      includeMomentumY=false);

    // Jeden Zeitschritt speichern
    writer.writeTimeStep(t, stride, h, hu, hv);

**Zum Lesen:**

.. code-block:: cpp

    // Existierende Datei laden
    io::NetCdf reader("solution.nc");
    
    int numSteps = reader.getTimeStepCount();
    
    for(int i = 0; i < numSteps; i++) {
      reader.readTimeStep(i, h, hu, hv);  // Daten laden
      // Jetzt kannst du die Daten prüfen oder verarbeiten
    }

**Was bekommst du?**

- Die Datei ``solution.nc`` mit allen Zeitschritten
- Jeder Zeitschritt hat die Variablen: ``height``, ``momentum_x``, ``momentum_y`` (optional)
- Koordinaten: ``x``, ``y``, ``time``
- Metadaten (Einheiten, Beschreibung) – damit ParaView es versteht

Praktisches Beispiel
====================

**1. Simulator startet und speichert:**

Der Code in ``main.cpp`` macht automatisch:

.. code-block:: cpp

    // Am Anfang
    io::NetCdf writer("solution.nc", dxy, nx, ny, false);
    
    // Schreibe den Anfangszustand
    writer.writeTimeStep(0, nx, h, hu, hv);
    
    // Später in der Zeitschleife
    while(time < t_end) {
        // ... simuliere einen Schritt ...
        writer.writeTimeStep(time, nx, h, hu, hv);
    }
    // Datei wird automatisch geschlossen

**Ergebnis:** Eine ``solution.nc`` Datei entsteht mit allen Daten.

**2. Später: Daten validieren**

Wenn die Simulation vorbei ist, kannst du testen:

.. code-block:: cpp

    io::NetCdf reader("solution.nc");
    
    int timesteps = reader.getTimeStepCount();
    float t0 = reader.getTime(0);
    
    // Lade Zeitschritt 0 und prüfe die Werte
    reader.readTimeStep(0, h, hu, hv);
    // Jetzt kannst du h, hu, hv prüfen mit assert() oder print

NetCDF Datei prüfen
===================

**Nach der Simulation hast du ``solution.nc``. Wie guckst du rein?**

``ncdump`` ist ein Tool, das NetCDF-Dateien ausliest:

.. code-block:: bash

    # Zeige die Struktur (Header)
    ncdump -h solution.nc
    
    # Zeige nur die Zeitvariable
    ncdump -v time solution.nc
    
    # Zeige alles (könnte viel sein!)
    ncdump solution.nc

**Beispiel-Output:**

.. code-block:: 

    netcdf solution {
    dimensions:
            time = UNLIMITED ;
            y = 50 ;
            x = 100 ;
    variables:
            float time(time) ;
                    time:units = "seconds since the earthquake event" ;
            float height(time, y, x) ;
                    height:units = "m" ;
    ...
    data:
     time = 0, 0.1, 0.2, ... ;
     height = ... ;
    }

**Wenn ``ncdump`` nicht installiert ist:**
- Installiere netCDF-Tools (z. B. über ein Paketmanagement oder per Python)
- Oder: Nutze ParaView direkt (siehe unten)

Visualisierung mit ParaView
===========================

Das Schöne: ParaView versteht NetCDF direkt!

**Schritte:**

1. ParaView starten
2. ``File`` → ``Open`` → ``solution.nc`` auswählen
3. Im ``Properties`` Panel:
   - Auswählen: ``Height`` (oder ``Momentum X``)
   - ``Apply`` klicken
4. Oben ``Play`` drücken → Animation sehen!

.. figure:: paraview_height_animation.gif
   :alt: ParaView animiert die Welle
   :align: center
   :width: 70%

   So schaut es aus: Welle wandert über die Domäne, ParaView malt die Höhe mit Farben.

**Was siehst du?**
- Die Wasserhöhe als Farbkarte (blau = niedrig, rot = hoch)
- Jeden Zeitschritt nacheinander
- Die Welle ausbreiten oder Reflexionen an Grenzen

Warum NetCDF? (Die Entscheidungen erklärt)
==========================================

**Binärformat statt CSV?**

CSV ist Text, alle Zahlen werden als Zeichen gespeichert → größere Dateien und langsameres Einlesen.

NetCDF speichert Binärdaten und ist in vielen Fällen effizienter bei Größe und I/O.

**Standard (COARDS)?**



Mit COARDS schreiben wir:
- Einheiten korrekt hin (``units = "m"``)
- Beschreibungen mit (``long_name = "water height"``)
- Zeit richtig (``time:units = "seconds since start"``)

Ergebnis: ParaView und andere Tools **verstehen sofort**, was die Daten bedeuten.

**Optional?**

Die Klasse ist mit ``#ifdef USE_NETCDF`` geschrieben.

- ``netcdf=auto`` → Nutzt NetCDF wenn vorhanden, ignoriert sonst
- ``netcdf=off`` → Keine NetCDF-Ausgabe (aber Code kompiliert)

Vorteil: Portable Builds, auch auf Systemen ohne NetCDF installiert.

**Ghost-Zellen ausschließen?**

Ghost-Zellen sind "Rand"-Zellen für numerische Berechnungen – nicht physikalisch.

Wir schreiben sie **nicht** raus:
- Kleinere Datei
- Klarere Visualisierung (keine Artefakte an Grenzen)
- ParaView zeigt nur die echte Physik

Zusammenfassung
===============

**Was hast du jetzt?**

✓ Der Simulator speichert Ergebnisse in ``solution.nc``  
✓ Die Datei ist klein und schnell (Binärformat)  
✓ ParaView kann die Datei direkt visualisieren  
✓ Du kannst alte Ergebnisse laden und prüfen  
✓ Alles funktioniert auch ohne NetCDF installiert  

**Konkret kannst du jetzt:**

- Eine Tsunami-Simulation starten → bekommst eine ``solution.nc`` Datei
- Diese Datei in ParaView öffnen → Animation der Welle anschauen
- Die Ergebnisse später wieder laden → zum Validieren oder Vergleichen
- Professionelle, publikationsreife Visualisierungen machen

**Nächste Schritte?**

1. Build mit ``netcdf=auto``
2. Simulator laufen lassen
3. ``ncdump`` oder ParaView nutzen
