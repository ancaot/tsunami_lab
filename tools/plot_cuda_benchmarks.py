"""Create presentation plots from the final F-Wave benchmark CSV."""

from __future__ import annotations

import csv
from pathlib import Path

import matplotlib.pyplot as plt


ROOT = Path(__file__).resolve().parents[1]
INPUT = ROOT / "outputs" / "cuda" / "fwave_final_benchmark.csv"
OUTPUT = ROOT / "sphinx" / "source" / "_static"


def number(value: str) -> float:
    return float(value.replace(",", "."))


with INPUT.open(encoding="utf-8-sig", newline="") as handle:
    rows = list(csv.DictReader(handle))

grids = [int(row["grid_size"]) for row in rows]
labels = [f"{size}²" for size in grids]
serial = [number(row["serial_ms"]) for row in rows]
openmp = [number(row["openmp_ms"]) for row in rows]
h2d = [number(row["h2d_ms"]) for row in rows]
kernel = [number(row["cuda_kernel_ms"]) for row in rows]
d2h = [number(row["d2h_ms"]) for row in rows]
cuda_e2e = [number(row["cuda_e2e_ms"]) for row in rows]
openmp_speedup = [number(row["openmp_speedup"]) for row in rows]
kernel_speedup = [number(row["cuda_kernel_speedup"]) for row in rows]
e2e_speedup = [number(row["cuda_e2e_speedup"]) for row in rows]

OUTPUT.mkdir(parents=True, exist_ok=True)
plt.style.use("seaborn-v0_8-whitegrid")


fig, axis = plt.subplots(figsize=(9, 5.2))
axis.plot(labels, serial, "o-", linewidth=2, label="CPU seriell")
axis.plot(labels, openmp, "o-", linewidth=2, label="OpenMP (16 Threads)")
axis.plot(labels, kernel, "o-", linewidth=2, label="CUDA-Kernel")
axis.plot(labels, cuda_e2e, "o-", linewidth=2, label="CUDA inkl. Transfers")
axis.set_yscale("log")
axis.set_xlabel("Gittergröße")
axis.set_ylabel("Laufzeit [ms], logarithmisch")
axis.set_title("F-Wave-Laufzeiten nach Gittergröße")
axis.legend()
fig.tight_layout()
fig.savefig(OUTPUT / "cuda_fwave_laufzeiten.png", dpi=180)
plt.close(fig)


fig, axis = plt.subplots(figsize=(9, 5.2))
axis.plot(labels, openmp_speedup, "o-", linewidth=2, label="OpenMP")
axis.plot(labels, kernel_speedup, "o-", linewidth=2, label="CUDA-Kernel")
axis.plot(labels, e2e_speedup, "o-", linewidth=2, label="CUDA inkl. Transfers")
axis.axhline(1.0, color="black", linewidth=1, linestyle="--", label="kein Speedup")
axis.set_yscale("log")
axis.set_xlabel("Gittergröße")
axis.set_ylabel("Speedup gegenüber CPU seriell")
axis.set_title("F-Wave-Speedup gegenüber der seriellen CPU")
axis.legend()
fig.tight_layout()
fig.savefig(OUTPUT / "cuda_fwave_speedup.png", dpi=180)
plt.close(fig)


fig, axis = plt.subplots(figsize=(9, 5.2))
axis.bar(labels, h2d, label="CPU → GPU")
axis.bar(labels, kernel, bottom=h2d, label="CUDA-Kernel")
bottom = [left + middle for left, middle in zip(h2d, kernel)]
axis.bar(labels, d2h, bottom=bottom, label="GPU → CPU")
axis.set_xlabel("Gittergröße")
axis.set_ylabel("CUDA-End-to-End-Zeit [ms]")
axis.set_title("Aufteilung der CUDA-End-to-End-Zeit")
axis.legend()
fig.tight_layout()
fig.savefig(OUTPUT / "cuda_fwave_transferkosten.png", dpi=180)
plt.close(fig)

print(f"Created three plots in {OUTPUT}")
