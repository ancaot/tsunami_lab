#!/bin/bash
#SBATCH --job-name=waveprop-cuda
#SBATCH --partition=gpu
#SBATCH --ntasks=1
#SBATCH --gres=gpu:1
#SBATCH --cpus-per-task=16
#SBATCH --output=waveprop_cuda.%j.out
#SBATCH --error=waveprop_cuda.%j.err
#SBATCH --time=02:00:00

set -euo pipefail

echo "Job started at: $(date)"

module load nvidia/cuda/12.1.0

nvcc -std=c++17 -O2 -arch=sm_80 cuda/wavepropagation2d_benchmark.cu src/patches/wavepropagation2d/WavePropagation2d.cpp src/solvers/FWave.cpp src/solvers/Roe.cpp -Xcompiler -fopenmp -o waveprog_benchmark

for GRID_SIZE in 128 256 512 1024 2048; do

    echo
    echo "Benchmarking ${GRID_SIZE}x${GRID_SIZE} grid..."

    echo OMP_NUM_THREADS=16 ./fwave_benchmark ${GRID_SIZE} 100 2>&1

done

echo
echo "Job finished at: $(date)"