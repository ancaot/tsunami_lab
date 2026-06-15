#!/bin/bash
#SBATCH --job-name=tsunami
#SBATCH --partition=short
#SBATCH --ntasks=1
#SBATCH --output=tsunami.out.%j
#SBATCH --error=tsunami.err.%j
#SBATCH --time=10:00
#SBATCH --cpus-per-task=96
echo "Job startet at: $(date)"
scons
./build/tsunami_lab << EOF
yes
EOF
echo "Job finished at: $(date)"
