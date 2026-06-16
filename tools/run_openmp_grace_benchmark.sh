#!/bin/bash
#SBATCH --job-name=tsunami-omp
#SBATCH --partition=short
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=144
#SBATCH --output=tsunami_omp.%j.out
#SBATCH --error=tsunami_omp.%j.err
#SBATCH --time=02:00:00

set -euo pipefail

echo "Job started at: $(date)"
echo "Host: $(hostname)"

scons --clean >/dev/null 2>&1 || true
scons mode=release opt=O3 openmp=true netcdf=on

mkdir -p outputs/omp_benchmarks

for schedule in static dynamic guided; do
  export OMP_SCHEDULE="${schedule}"
  for binding in close spread; do
    export OMP_PROC_BIND="${binding}"
    export OMP_PLACES=cores
    for threads in 1 2 4 8 16 32 64 96 128 144; do
      rm outputs/checkpoints/checkpoint.nc
      export OMP_NUM_THREADS="${threads}"
      log="outputs/omp_benchmarks/grace_t${threads}_${schedule}_${binding}.log"
      echo "threads=${threads} schedule=${schedule} bind=${binding}"
      ./build/tsunami_lab << EOF | tee "${log}"
yes
EOF
    done
  done
done

echo "Job finished at: $(date)"
