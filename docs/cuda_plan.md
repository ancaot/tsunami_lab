# CUDA-Setup und erster technischer Fokus

Diese Notiz dokumentiert den ersten CUDA-Schritt im `tsunami_lab`-Projekt. Das
Ziel für diese Woche ist noch keine vollständige GPU-Portierung. Stattdessen
soll gezeigt werden, dass CUDA lokal kompiliert und ausgeführt werden kann,
welche Codebereiche besonders relevant sind und welcher numerische Kern zuerst
als Prototyp auf die GPU gebracht werden sollte.

## Aktuelle Umgebung

- GPU mit `nvidia-smi` erkannt: NVIDIA GeForce RTX 4060, 8 GB VRAM.
- NVIDIA-Treiber erkannt: 596.49.
- Der Treiber meldet CUDA-Laufzeitunterstützung bis Version 13.2.
- CUDA Toolkit über `winget` installiert: `Nvidia.CUDA` Version 13.3.
- CUDA-Compiler installiert:
  `C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v13.3\bin\nvcc.exe`.
- `nvcc --version`: CUDA compilation tools release 13.3, V13.3.33.
- Visual Studio Build Tools sind installiert, aber `cl.exe` liegt nicht im
  normalen PowerShell-`PATH`. Das Smoke-Test-Skript lädt die Visual-Studio-
  Build-Tools-Umgebung deshalb automatisch.
- Die lokale RTX 4060 ist eine Ada-GPU. Das Smoke-Test-Skript verwendet
  `-arch=sm_89`, damit `nvcc` nativen Code für diese GPU erzeugt und nicht auf
  PTX-JIT-Kompilierung durch den Treiber angewiesen ist.
- Ergebnis des Smoke-Tests: `CUDA smoke test passed for 1024 values.`

Wichtiger Unterschied: `nvidia-smi` bestätigt, dass der Treiber die GPU sieht.
Das bedeutet aber noch nicht automatisch, dass CUDA-Code kompiliert werden kann.
Dafür braucht man das CUDA Toolkit, besonders `nvcc.exe`.

## CUDA Toolkit unter Windows einrichten

1. CUDA Toolkit von <https://developer.nvidia.com/cuda/toolkit> installieren.
   Mit `winget` geht das so:

   ```powershell
   winget install --id Nvidia.CUDA --exact
   ```

2. Während der Installation die Visual-Studio-Integration aktiviert lassen,
   wenn Visual Studio Build Tools installiert sind.
3. Nach der Installation eine neue PowerShell öffnen, damit `CUDA_PATH` und
   `PATH` aktualisiert werden.
4. Installation prüfen:

   ```powershell
   nvidia-smi
   nvcc --version
   ```

5. Smoke-Test im Repository bauen und ausführen:

   ```powershell
   .\tools\build_cuda_smoke.ps1
   ```

   Auf einer anderen NVIDIA-GPU muss eventuell die passende Architektur
   angegeben werden, zum Beispiel:

   ```powershell
   .\tools\build_cuda_smoke.ps1 -CudaArch sm_86
   ```

Falls `nvcc` in einem anderen Verzeichnis installiert ist, kann der CUDA-Pfad
explizit übergeben werden:

```powershell
.\tools\build_cuda_smoke.ps1 -CudaPath "C:\Path\To\CUDA\v13.2"
```

## Erster CUDA-Smoke-Test

Die Datei `cuda/smoke_test.cu` enthält einen minimalen CUDA-Kernel:

- drei Arrays auf der GPU allokieren,
- Eingabedaten von der CPU auf die GPU kopieren,
- einen Kernel mit vielen CUDA-Threads starten,
- das Ergebnis zurück auf die CPU kopieren,
- jedes Ergebnis mit der CPU-Erwartung vergleichen.

Dieser Test ist bewusst unabhängig von SCons. Er beantwortet zuerst die
Grundfrage: "Kann diese Maschine CUDA-Code kompilieren und ausführen?"

## Relevante Codebereiche

Der aktuelle zweidimensionale Solver liegt hier:

- `src/patches/wavepropagation2d/WavePropagation2d.cpp`
- `src/patches/wavepropagation2d/WavePropagation2d.h`
- `src/solvers/FWave.cpp`
- `src/solvers/fwave.h`

`WavePropagation2d::timeStep` arbeitet aktuell in zwei Richtungen:

1. alte Arrays in den nächsten Buffer kopieren,
2. Kanten-Updates in x-Richtung berechnen,
3. Ghost Cells aktualisieren,
4. erneut kopieren,
5. Kanten-Updates in y-Richtung berechnen,
6. Ghost Cells erneut aktualisieren.

Der rechenintensive Teil liegt in den x- und y-Kantenschleifen. Jede Kante ruft
entweder `Roe::netUpdates` oder `fwave::netUpdates` auf. Beim f-wave Solver wird
dieselbe lokale Berechnung für viele voneinander unabhängige Kanten wiederholt.

## Entscheidung: zuerst fwave::netUpdates portieren

Der empfohlene erste numerische CUDA-Prototyp ist eine Batch-Version von
`fwave::netUpdates`, nicht sofort das gesamte `WavePropagation2d::timeStep`.

Gründe:

- `fwave::netUpdates` ist ein kleiner, klar abgegrenzter numerischer Kern.
- Ein CUDA-Thread kann genau ein Kanten-Update berechnen.
- Eingaben und Ausgaben sind einfache `float`-Arrays. Dadurch lässt sich die
  CUDA-Version gut mit der CPU-Version vergleichen.
- Man muss sich am Anfang noch nicht mit dem kompletten Buffer-Wechsel,
  Ghost-Cell-Updates und möglichen Schreibkonflikten zwischen Nachbarzellen
  beschäftigen.

`WavePropagation2d::timeStep` sollte erst danach portiert werden, wenn der
batched f-wave Kernel korrekt funktioniert. Der vollständige Zeitschritt ist
zwar das eigentliche Performance-Ziel, hat aber mehr bewegliche Teile und ist
deshalb als erster CUDA-Schritt riskanter.

## Nächster Implementierungsschritt

Nach dem erfolgreichen CUDA-Smoke-Test:

1. CUDA-Variante `fwaveNetUpdatesKernel` erstellen, bei der jeder Thread eine
   Kante berechnet,
2. CPU-Testarrays mit mehreren Kanten-Zuständen vorbereiten,
3. CPU-Version `fwave::netUpdates` und CUDA-Version ausführen,
4. Ergebnisarrays mit einer kleinen Floating-Point-Toleranz vergleichen,
5. erst danach den Kernel in `WavePropagation2d` einbinden.

## F-Wave-Prototyp

Der erste numerische Prototyp liegt in `cuda/fwave_benchmark.cu`. Ein CUDA-
Thread berechnet die vier Net-Updates einer Kante. Das Programm allokiert zehn
Arrays auf der GPU, kopiert sechs Eingabearrays auf die GPU, führt den Kernel
aus und kopiert vier Ergebnisarrays zur CPU zurück. Danach werden alle Werte
mit der produktiven CPU-Implementierung `fwave::netUpdates` verglichen.

Build, Vergleich und Benchmark werden gemeinsam gestartet:

```powershell
.\tools\build_cuda_fwave.ps1
```

Problemgröße und Anzahl der Messwiederholungen sind konfigurierbar:

```powershell
.\tools\build_cuda_fwave.ps1 -Edges 100000 -Iterations 50
```

Ausgegeben werden GPU-Allokationszeit, CPU-Laufzeit, H2D-Transfer, mittlere
Kernel-Laufzeit, D2H-Transfer, Kernel-Speedup, End-to-End-Speedup und maximale
Abweichung. Ein Warm-up-Aufruf verhindert, dass die einmalige CUDA-
Initialisierung die Kernelmessung verfälscht.

### Messergebnis vom 29.06.2026

Testsystem: NVIDIA GeForce RTX 4060, eine Million Kanten, 100 Kernel-
Wiederholungen, Release-Build für `sm_89`:

| Messwert | Ergebnis |
|---|---:|
| CPU `fwave::netUpdates` | 14,711 ms |
| H2D-Transfer | 4,354 ms |
| CUDA-Kernel | 0,176 ms |
| D2H-Transfer | 2,789 ms |
| CUDA H2D + Kernel + D2H | 7,319 ms |
| reiner Kernel-Speedup | 83,707x |
| End-to-End-Speedup | 2,010x |
| geprüfte Ausgabewerte | 4.000.000 |
| Abweichungen außerhalb der Toleranz | 0 |

Die GPU-Allokation von 40 MB dauerte beim ersten Lauf 79,547 ms und wird daher
nicht pro Zeitschritt wiederholt werden. Der große Unterschied zwischen
Kernel- und End-to-End-Speedup zeigt, dass Transfers den Prototyp dominieren.

### Erkenntnisse und nächster Schritt

- Rundungsunterschiede zwischen CPU und GPU erfordern einen Vergleich mit
  absoluter und relativer Toleranz; exakte Bitgleichheit ist nicht sinnvoll.
- Trockene Randzellen werden in der CPU-Version durch Umleiten eines lokalen
  Ausgabezeigers behandelt. Der CUDA-Kernel bildet dieses Verhalten explizit
  nach.
- Für `WavePropagation2d` sollen die Simulationsarrays dauerhaft im GPU-
  Speicher bleiben. Sonst wird der mögliche Kernel-Speedup durch PCIe-
  Transfers aufgezehrt.
- Als nächstes werden x- und y-Kanten separat parallelisiert. Zellupdates
  brauchen dabei eine konfliktfreie Strategie, beispielsweise getrennte
  Net-Update-Arrays mit anschließendem Zellkernel.
