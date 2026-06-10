#!/bin/bash
#SBATCH --job-name=tsunami_analysis
#SBATCH --partition=short
#SBATCH --ntasks=1
#SBATCH --output=tsunami_analysis.out.%j
#SBATCH --error=tsunami_analysis.err.%j
#SBATCH --time=30:00
#SBATCH --cpus-per-task=96
echo "Job startet at: $(date)"
module load intel/oneapi/2025.0.0
export PATH=/cluster/intel/oneapi/2025.0.0/vtune/2025.0/bin64:$PATH
scons
/cluster/intel/oneapi/2025.0.0/vtune/2025.0/bin64/vtune -collect hotspots --app-working-dir=/home/ne67fuh/tsunami_lab -- /home/ne67fuh/tsunami_lab/build/tsunami_lab << EOF
yes
EOF
echo "Job finished at: $(date)"
