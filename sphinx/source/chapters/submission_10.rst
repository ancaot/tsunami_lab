################################################
Submission 10: CUDA Optimierung/Parallelisierung
################################################

10.1 Plan
=========

1. Ziel
-------

In unserer finalen Projektphase möchten wir untersuchen, ob sich unser Tsunami
Simulationsprogramm mithilfe von CUDA auf GPUs beschleunigen lässt. In der vorherigen 
Abgabe haben wir bereits eine OpenMP-Parallelisierung für den zweidimensionalen Solver 
umgesetzt. Dabei lag der Fokus besonders auf WavePropagation2d, da dort ein großer 
Teil der Laufzeit entsteht. Aufbauend darauf möchten wir nun untersuchen, ob sich diese 
rechenintensiven Teile auch effizient auf NVIDIA-GPUs ausführen lassen. 

Unser Ziel ist es nicht, sofort das gesamte Programm inklusive IO, Konfiguration und 
NetCDF-Ausgabe vollständig auf CUDA umzubauen. Stattdessen wollen wir zuerst die 
numerischen Kernkomponenten betrachten. Besonders relevant sind dabei der f-wave 
Solver und die Zeitschrittberechnung im 2D-Patch. Diese Bereiche werden sehr häufig 
ausgeführt und eignen sich deshalb gut für Parallelisierung. 

Am Ende möchten wir vergleichen, wie sich die CUDA-Version gegenüber der bisherigen 
seriellen CPU-Version und der OpenMP-Version verhält. Dabei betrachten wir Laufzeit, 
Speedup, Speicherzugriffe und die Aufteilung der Arbeit zwischen CPU und GPU. 
Motivation Tsunami-Simulationen arbeiten mit großen Gittern. 

Für jedes Zeitschritt-Update müssen viele lokale Berechnungen auf den Zellen oder 
Zellkanten durchgeführt werden. Diese Berechnungen sind teilweise unabhängig 
voneinander und daher grundsätzlich gut parallelisierbar. OpenMP nutzt mehrere CPU
Kerne, während CUDA sehr viele GPU-Threads verwenden kann. Deshalb ist CUDA ein 
sinnvoller nächster Schritt nach unserer OpenMP-Parallelisierung. 

Besonders interessant ist die Frage, ab welcher Problemgröße sich CUDA lohnt. Für 
kleine Gitter kann der GPU-Overhead durch Speichertransfers und Kernel-Starts größer 
sein als der eigentliche Vorteil. Für größere Simulationen wie Tohoku oder Chile könnte 
CUDA jedoch deutliche Vorteile bringen.


2. Meilensteile
---------------

**1. Analyse und CUDA-Grundstruktur**

Zuerst analysieren wir die bestehende OpenMP-Implementierung und entscheiden, 
welcher Programmteil zuerst auf CUDA portiert wird. 

Wahrscheinliche Kandidaten sind: 

* fwave::netUpdates 
* WavePropagation2d::timeStep x- und y-Sweeps im 2D-Solver 

Zusätzlich prüfen wir, wie CUDA in unser bestehendes Build-System eingebunden werden 
kann. Falls eine vollständige Integration in SCons zu aufwendig ist, erstellen wir zunächst 
einen separaten CUDA-Prototyp.

**2. CUDA-Prototyp**

Im zweiten Schritt implementieren wir einen ersten CUDA-Kernel. Dieser soll einen klar 
abgegrenzten rechenintensiven Teil der Simulation übernehmen. Dabei müssen die 
relevanten Arrays auf die GPU kopiert, dort verarbeitet und anschließend wieder zurück 
auf die CPU übertragen werden. 

Mögliche erste Implementierung: viele unabhängige f-wave Updates parallel auf der GPU 
berechnen oder einen vereinfachten 2D-Sweep auf der GPU ausführen.

**3. Tests und Korrektheit**

Für alle geänderten oder neu implementierten Teile sollen Tests oder 
Plausibilitätsprüfungen durchgeführt werden. Die CUDA-Ergebnisse werden mit der 
bestehenden CPU-Version verglichen. Wegen Floating-Point-Unterschieden erwarten wir 
keine vollständig bitgenauen Ergebnisse, aber die Abweichungen sollten klein und 
erklärbar sein.

**4. Simulationsdurchführung**

Nach dem CUDA-Prototyp testen wir verschiedene Szenarien. Zunächst verwenden wir 
kleinere künstliche Setups, weil diese einfacher zu debuggen sind. Danach können wir, 
falls die CUDA-Version stabil läuft, größere Szenarien testen: 

* Tohoku in grober Auflösung
* Chile in grober Auflösung
* optional verschiedene Gittergrößen, z.B. 500 x 500, 1000 x 1000, 2000 x 2000

**5. Optimierung und Analyse**

Nach den ersten Simulationsläufen analysieren wir mögliche Performance-Probleme. 
Dabei betrachten wir: 

* Speichertransfers zwischen CPU und GPU
* Speicherzugriffsmuster auf der GPU
* Arbeitsverteilung auf GPU-Threads 
* mögliche Bottlenecks im Kernel
* Vergleich mit den bisherigen OpenMP-Hotspots 

Falls möglich, nutzen wir Profiling-Werkzeuge wie NVIDIA Nsight oder andere verfügbare 
Tools. VTune ist eher für CPU/OpenMP hilfreich, für CUDA wären NVIDIA-Tools 
wahrscheinlich passender.

**6. Vergleich CUDA vs. CPU/OpenMP**

Am Ende vergleichen wir: 

* serielle CPU-Version
* OpenMP-Version
* CUDA-Version.

Wichtige Vergleichsgrößen: 

* Laufzeit der Zeitschritt-Schleife 
* Speedup 
* Cell updates per second 
* Speichertransferkosten 
* Skalierung mit steigender Gittergröße 
* Genauigkeit bzw. Plausibilität der Ergebnisse


3. Work Packages
----------------

**WP1: Analyse der bestehenden OpenMP-Version**

Wir untersuchen, welche Teile der bisherigen OpenMP-Version für CUDA geeignet sind. 
Dabei konzentrieren wir uns besonders auf WavePropagation2d und den f-wave Solver. 

**WP2: CUDA-Setup und Build** 

Wir prüfen die CUDA-Umgebung und binden CUDA entweder direkt in das Build-System 
ein oder erstellen zunächst einen separaten Prototyp. Ziel ist, CUDA-Code zuverlässig 
kompilieren und ausführen zu können. 

**WP3: CUDA-Kernel-Implementierung**

Wir implementieren einen ersten CUDA-Kernel für einen numerischen Kern der 
Simulation. Dabei achten wir auf sinnvolle Thread- und Blockgrößen sowie auf möglichst 
einfache und nachvollziehbare Speicherzugriffe. 

**WP4: Datenübertragung und Speicherverwaltung**

Wir verwalten GPU-Speicher für die relevanten Arrays, z.B. Wasserhöhe, Impuls und 
Bathymetrie. Außerdem untersuchen wir, wie oft Daten zwischen CPU und GPU kopiert 
werden müssen. 

**WP5: Tests und Ergebnisvergleich**

Die CUDA-Ergebnisse werden mit der CPU-Version verglichen. Dabei prüfen wir, ob die 
Werte plausibel bleiben und ob numerische Abweichungen akzeptabel sind. 

**WP6: Benchmarking**

Wir messen die Laufzeiten verschiedener Varianten und erstellen Tabellen oder Plots für 
CPU, OpenMP und CUDA. Dabei testen wir verschiedene Gittergrößen und eventuell 
verschiedene Szenarien. 

**WP7: Dokumentation und Präsentation**

Wir dokumentieren die Implementierung, die Messergebnisse, Probleme und Grenzen. 
Außerdem bereiten wir die Statuspräsentationen und die finale Präsentation vor. 


4. Zeitplan
-----------

**Bis 25.06. – 1. Statuspräsentation**

*Ziele:*

* OpenMP-Arbeit als Ausgangspunkt zusammenfassen 
* CUDA-Ziel und technischen Fokus festlegen
* relevante Codebereiche analysieren 
* CUDA Toolkit / GPU-Umgebung prüfen 
* ersten kleinen CUDA-Testkernel erstellen 
* entscheiden, ob zuerst fwave::netUpdates oder ein Teil von WavePropagation2d::timeStep portiert wird 

*Präsentierbares Ergebnis:*

* Projektziel geplanter CUDA-Fokus 
* erste technische Einschätzung Risiken und nächster Schritt 

**Bis 02.07. – 2. Statuspräsentation** 

*Ziele:*

* ersten CUDA-Prototyp implementieren 
* Speicher auf GPU allokieren 
* Daten CPU → GPU und GPU → CPU übertragen 
* ersten numerischen Kernel ausführen 
* Ergebnisse mit CPU-Version vergleichen 
* erste kleine Laufzeitmessungen durchführen 

*Präsentierbares Ergebnis:*

* erster CUDA-Prototyp Beispielvergleich CPU vs. CUDA 
* erste Probleme oder Erkenntnisse Plan für finale Benchmarks

**Bis 09.07. – Finale Präsentation**

*Ziele:*

* CUDA-Version benchmarken 
* Vergleich: CPU seriell vs. OpenMP vs. CUDA 
* verschiedene Gittergrößen testen 
* Speedup berechnen 
* Speichertransferkosten diskutieren 
* Grenzen der Implementierung erklären 
* mögliche weitere Optimierungen nennen 

*Präsentierbares Ergebnis:*

* finale Laufzeittabellen 
* Speedup-Vergleich 
* Bewertung, ob CUDA für unsere Simulation sinnvoll ist 
* Lessons Learned 

**Bis 31.07. – Finale Abgabe**

*Ziele:*

* Bugfixes, Code aufräumen 
* Dokumentation vervollständigen
* finale Tabellen und Plots ergänzen  und schriftliche Auswertung fertigstellen 
* Tests finalisieren


5. Risiken
----------

Ein hohes Risiko ist, dass die vollständige CUDA-Integration in das bestehende Programm 
zu aufwendig wird. Deshalb planen wir zuerst einen begrenzten CUDA-Prototyp für einen 
klar definierten Rechenkern. 

Ein weiteres Risiko sind Speichertransfers zwischen CPU und GPU. Wenn zu viele Daten 
pro Zeitschritt kopiert werden müssen, kann der Speedup stark reduziert werden. 
Außerdem ist das aktuelle Datenlayout eventuell nicht optimal für GPU-Zugriffe. CUDA 
profitiert von zusammenhängenden und gut strukturierten Speicherzugriffen. Falls die 
Speicherzugriffe ungünstig sind, kann die GPU trotz vieler Threads ineffizient arbeiten. 

Die Korrektheit ist auch ein Risiko. Durch andere Ausführungsreihenfolgen und Floating
Point-Unterschiede können CUDA-Ergebnisse leicht von CPU-Ergebnissen abweichen. 
Diese Unterschiede müssen analysiert und eingeordnet werden. 


6. Erwartetes Ergebnis
----------------------

Am Ende erwarten wir eine Einschätzung, ob CUDA für unsere Tsunami-Simulation 
sinnvoll ist. Im besten Fall erreichen wir einen messbaren Speedup gegenüber der 
seriellen CPU-Version und können CUDA auch mit OpenMP vergleichen. Falls der 
Speedup kleiner ausfällt als erwartet, ist das trotzdem ein wichtiges Ergebnis, weil wir 
dann besser verstehen, welche Teile unseres Codes GPU-freundlich sind und welche 
Änderungen für zukünftige Optimierungen notwendig wären. 


7. Ressourcen
-------------

* `CUDA Toolkit <https://developer.nvidia.com/cuda/toolkit>`
* `NVIDIA CUDA Dokumentation <https://docs.nvidia.com/cuda/>`
* bestehende OpenMP-Implementierung aus Submission 9 
* bestehende Benchmark-Szenarien Tohoku und Chile 
* vorhandene Laufzeitmessungen der Zeitschritt-Schleife 


10.2 Erstes Status-Update (18.06. bis 25.06.)
=============================================

1. Vorgenommene Ziel
--------------------

* OpenMP-Arbeit als Ausgangspunkt zusammenfassen 
* CUDA-Ziel und technischen Fokus festlegen
* relevante Codebereiche analysieren 
* CUDA Toolkit / GPU-Umgebung prüfen 
* ersten kleinen CUDA-Testkernel erstellen 
* entscheiden, ob zuerst fwave::netUpdates oder ein Teil von WavePropagation2d::timeStep portiert wird


2. OpenMP Ausgangspunkt und OpenMP-Hotspots des parallelisierten Programms
--------------------------------------------------------------------------

