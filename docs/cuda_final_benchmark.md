# Finaler CUDA-Benchmark des F-Wave-Kerns

## Gegenstand und Testsystem

Gemessen wird die Batch-Version von `fwave::netUpdates`, nicht der komplette
`WavePropagation2d::timeStep`. Eine Kante erzeugt vier Net-Updates. Fuer ein
quadratisches Gitter mit `N x N` Zellen werden `2 * N * (N + 1)` horizontale
und vertikale Kanten angesetzt.

- CPU: AMD Ryzen 7 5800X, 8 Kerne / 16 logische Prozessoren
- GPU: NVIDIA GeForce RTX 4060, 8 GB
- CUDA: 13.3, Zielarchitektur `sm_89`
- Datentyp: `float`
- CPU-Build: `/O2`
- OpenMP: statisches Scheduling, 16 Threads
- CUDA: 256 Threads je Block, 100 Messwiederholungen nach einem Warm-up
- Datum: 03.07.2026

## Laufzeittabelle

Alle Zeiten sind in Millisekunden angegeben. CUDA End-to-End umfasst H2D,
einen Kernel und D2H, aber nicht die einmalige GPU-Allokation.

| Gitter | Kanten | Seriell | OpenMP | H2D | CUDA-Kernel | D2H | CUDA End-to-End |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 128 x 128 | 33.024 | 0,412 | 0,094 | 0,286 | 0,013 | 0,251 | 0,550 |
| 256 x 256 | 131.584 | 1,705 | 0,419 | 0,960 | 0,014 | 0,573 | 1,547 |
| 512 x 512 | 525.312 | 7,300 | 1,072 | 2,634 | 0,029 | 1,734 | 4,397 |
| 1024 x 1024 | 2.099.200 | 29,496 | 6,869 | 8,674 | 0,373 | 5,637 | 14,683 |
| 2048 x 2048 | 8.392.704 | 121,004 | 21,112 | 32,068 | 1,482 | 21,058 | 54,607 |

## Speedups

Speedup ist `T_seriell / T_parallel`. Ein Wert groesser als 1 bedeutet einen
Gewinn gegenueber der seriellen CPU. `CUDA vs. OpenMP` ist
`T_OpenMP / T_CUDA-End-to-End`; Werte kleiner als 1 bedeuten, dass OpenMP in
diesem Transfermodell schneller ist.

| Gitter | OpenMP vs. seriell | CUDA-Kernel vs. seriell | CUDA E2E vs. seriell | CUDA E2E vs. OpenMP |
|---:|---:|---:|---:|---:|
| 128 x 128 | 4,40x | 31,87x | 0,75x | 0,17x |
| 256 x 256 | 4,07x | 118,86x | 1,10x | 0,27x |
| 512 x 512 | 6,81x | 255,14x | 1,66x | 0,24x |
| 1024 x 1024 | 4,29x | 79,14x | 2,01x | 0,47x |
| 2048 x 2048 | 5,73x | 81,66x | 2,22x | 0,39x |

## Korrektheit

OpenMP liefert fuer alle getesteten Werte exakt dasselbe Ergebnis wie die
serielle CPU. CUDA wird mit absoluter und relativer Toleranz verglichen. In
allen Gittergroessen gab es null Werte ausserhalb der Toleranz. Die maximale
absolute Abweichung betrug `6,104e-05`, die maximale relative Abweichung
`8,719e-04`.

## Bewertung

Der F-Wave-Kern ist sehr gut GPU-parallelisierbar. Wenn die Daten bereits auf
der GPU liegen, ist der Kernel je nach Gitter in dieser Messung etwa 32- bis
255-mal schneller als die serielle CPU und etwa 7- bis 37-mal schneller als
OpenMP. Die Werte kleiner Gitter profitieren bei den wiederholten Kernelstarts
auch von GPU-Caches und sind daher nicht als Speedup einer ganzen Simulation zu
interpretieren.

Werden fuer jeden einzelnen Kernel sechs Eingabearrays auf die GPU und vier
Ausgabearrays zurueck zur CPU kopiert, dominieren die PCIe-Transfers. Beim
2048er-Gitter braucht der Kernel nur 1,482 ms, die Transfers zusammen aber
53,126 ms. CUDA ist dann zwar 2,22-mal schneller als die serielle CPU, aber
OpenMP bleibt etwa 2,59-mal schneller als CUDA End-to-End.

CUDA ist fuer die Simulation sinnvoll, wenn mehrere Rechenschritte auf der GPU
verkettet werden und die Zustandsarrays dauerhaft im GPU-Speicher bleiben. Eine
Portierung nur einer einzelnen Funktion mit Hin- und Ruecktransfer bei jedem
Aufruf ist gegenueber der vorhandenen OpenMP-Version nicht sinnvoll.

## Grenzen und weitere Optimierungen

- Portiert ist nur `fwave::netUpdates`; Bufferwechsel, Zellupdates, Ghost Cells,
  Randbedingungen und der komplette 2D-Zeitschritt fehlen noch.
- Synthetische Eingaben ersetzen noch keinen Benchmark einer realen
  Tsunami-Konfiguration.
- Die Messung verwendet eine einzelne CPU/GPU-Kombination und ist nicht direkt
  auf andere Systeme uebertragbar.
- Wiederholte Kernel arbeiten auf denselben Arrays; besonders kleine
  Problemgroessen koennen dadurch von GPU-Caches profitieren.
- GPU-Speicher wird im Prototyp als zehn getrennte Arrays belegt. Fuer die
  Gesamtsimulation muss der Speicherbedarf mit allen Zeitschritt- und
  Kantenpuffern geplant werden.
- Naechster Hauptschritt: Simulationsarrays einmal auf die GPU kopieren, x- und
  y-Richtung dort berechnen und nur fuer Ausgabe oder Checkpoints zurueckkopieren.
- Konfliktfreie Zellupdates koennen in einem zweiten Kernel aus getrennten
  Kantenupdates gesammelt werden; damit werden atomare Operationen vermieden.
- CUDA Streams, asynchrone Transfers und Pinned Memory koennen unvermeidbare
  Transfers mit Berechnung ueberlappen.
- Kernel-Fusion kann Zwischenarrays und zusaetzliche Speicherzugriffe reduzieren.
- Blockgroesse, Speicherlayout und Speicherzugriffe sollten mit Nsight Compute
  profiliert und danach gezielt optimiert werden.

## Reproduzieren

```powershell
.\tools\run_cuda_fwave_benchmarks.ps1
```

Die Rohdaten werden nach `outputs/cuda/fwave_final_benchmark.csv` geschrieben.
