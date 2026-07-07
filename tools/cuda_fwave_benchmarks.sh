#!/bin/bash
#SBATCH --job-name=fwave-cuda
#SBATCH --partition=gpu
#SBATCH --ntasks=1
#SBATCH --gres=gpu:1
#SBATCH --cpus-per-task=16
#SBATCH --output=fwave_cuda.%j.out
#SBATCH --error=fwave_cuda.%j.err
#SBATCH --time=02:00:00

set -euo pipefail

echo "Job started at: $(date)"

module load nvidia/cuda/12.1.0

GRID_SIZES=(128 256 512 1024 2048)
ITERATIONS=100
CUDA_ARCH="sm_80"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(realpath "$SCRIPT_DIR/..")"

BUILD_SCRIPT="nvcc -std=c++17 -O2 cuda/fwave_benchmark.cu src/solvers/FWave.cpp -o fwave_benchmark"
EXECUTABLE="$REPO_ROOT/build/cuda/fwave_benchmark"
OUTPUT_DIR="$REPO_ROOT/outputs/cuda"
CSV_PATH="$OUTPUT_DIR/fwave_final_benchmark.csv"

$BUILD_SCRIPT -cuda-arch "$CUDA_ARCH" -build-only

mkdir -p "$OUPUT_DIR"

echo "grid_size,edges,serial_ms,openmp_ms,h2d_ms,cuda_kernel_ms,d2h_ms,cuda_e2e_ms,openmp_speedup,cuda_kernel_speedup,cuda_e2e_speedup,cuda_vs_openmp,cuda_mismatches,openmp_mismatches" > "$CSV_PATH"

for GRID_SIZE in "${GRID_SIZES[@]}"; do
    EDGES=$((2 * GRID_SIZE * (GRID_SIZE + 1)))

    echo
    echo "Benchmarking ${GRID_SIZE}x${GRID_SIZE} grid (${EDGES} edges)..."

    OUTPUT=$("$EXECUTABLE" "$EDGES" "$ITERATIONS" 2>&1)

    echo "$OUTPUT"

    CSV_LINE=$(echo "$OUTPUT" | grep '^CSV,' | tail -n1)

    if [[ -z "$CSV_LINE" ]]; then
        echo "CSV result missing for grid size $GRID_SIZE."
        exit 1
    fi

    IFS=',' read -r _ \
        edges \
        serial_ms \
        openmp_ms \
        h2d_ms \
        cuda_kernel_ms \
        d2h_ms \
        cuda_e2e_ms \
        openmp_speedup \
        cuda_kernel_speedup \
        cuda_e2e_speedup \
        cuda_vs_openmp \
        cuda_mismatches \
        openmp_mismatches <<< "$CSV_LINE"

    echo "$GRID_SIZE,$edges,$serial_ms,$openmp_ms,$h2d_ms,$cuda_kernel_ms,$d2h_ms,$cuda_e2e_ms,$openmp_speedup,$cuda_kernel_speedup,$cuda_e2e_speedup,$cuda_vs_openmp,$cuda_mismatches,$openmp_mismatches" >> "$CSV_PATH"
done

echo
echo "Results written to $CSV_PATH"