#!/bin/bash

export OMP_SCHEDULE=static
export OMP_PROC_BIND=true
export OMP_DYNAMIC=false
#export OMP_DISPLAY_AFFINITY=true
#export OMP_DISPLAY_ENV=true

for N in 128 256 512; do

for r in 1 2 3 4 5; do
    bsub -J "launch omp" -n 1 -o logs/log.out.${N}.0.1  -e logs/log.err.${N}.0.1  -x -R "span[hosts=1] affinity[core(1)]"  -m "polus-c3-ib polus-c4-ib" OMP_NUM_THREADS=1 /polusfs/lsf/openmp/launchOpenMP.py ./prog_omp -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
    bsub -J "launch omp" -n 1 -o logs/log.out.${N}.0.2  -e logs/log.err.${N}.0.2  -x -R "span[hosts=1] affinity[core(2)]"  -m "polus-c3-ib polus-c4-ib" OMP_NUM_THREADS=2 /polusfs/lsf/openmp/launchOpenMP.py ./prog_omp -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 2
    bsub -J "launch omp" -n 1 -o logs/log.out.${N}.0.4  -e logs/log.err.${N}.0.4  -x -R "span[hosts=1] affinity[core(4)]"  -m "polus-c3-ib polus-c4-ib" OMP_NUM_THREADS=4 /polusfs/lsf/openmp/launchOpenMP.py ./prog_omp -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 4
    bsub -J "launch omp" -n 1 -o logs/log.out.${N}.0.8  -e logs/log.err.${N}.0.8  -x -R "span[hosts=1] affinity[core(8)]"  -m "polus-c3-ib polus-c4-ib" OMP_NUM_THREADS=8 /polusfs/lsf/openmp/launchOpenMP.py ./prog_omp -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 8
    bsub -J "launch omp" -n 1 -o logs/log.out.${N}.0.16 -e logs/log.err.${N}.0.16 -x -R "span[hosts=1] affinity[core(16)]" -m "polus-c3-ib polus-c4-ib" OMP_NUM_THREADS=16 /polusfs/lsf/openmp/launchOpenMP.py ./prog_omp -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 16
    bsub -J "launch omp" -n 1 -o logs/log.out.${N}.0.32 -e logs/log.err.${N}.0.32 -x -R "span[hosts=1] affinity[core(16)]" -m "polus-c3-ib polus-c4-ib" OMP_NUM_THREADS=32 /polusfs/lsf/openmp/launchOpenMP.py ./prog_omp -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 32
done

for r in 1 2 3 4 5; do
    bsub -J "launch mpi" -n 1  -o logs/log.out.${N}.1.1  -e logs/log.err.${N}.1.1  -x -R "span[hosts=1]"  -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
    bsub -J "launch mpi" -n 2  -o logs/log.out.${N}.2.1  -e logs/log.err.${N}.2.1  -x -R "span[ptile=1]"  -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
    bsub -J "launch mpi" -n 4  -o logs/log.out.${N}.4.1  -e logs/log.err.${N}.4.1  -x -R "span[ptile=2]"  -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
    bsub -J "launch mpi" -n 8  -o logs/log.out.${N}.8.1  -e logs/log.err.${N}.8.1  -x -R "span[ptile=4]"  -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
    bsub -J "launch mpi" -n 16 -o logs/log.out.${N}.16.1 -e logs/log.err.${N}.16.1 -x -R "span[ptile=8]"  -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
    bsub -J "launch mpi" -n 32 -o logs/log.out.${N}.32.1 -e logs/log.err.${N}.32.1 -x -R "span[ptile=16]" -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
done

for r in 1 2 3 4 5; do
    bsub -J "launch mpi_omp" -n 1 -o logs/log.out.${N}.1.4 -e logs/log.err.${N}.1.4 -x -R "span[hosts=1] affinity[core(2)]" -m "polus-c3-ib polus-c4-ib" OMP_NUM_THREADS=4 mpiexec ./prog_hyb -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 4
    bsub -J "launch mpi_omp" -n 2 -o logs/log.out.${N}.2.4 -e logs/log.err.${N}.2.4 -x -R "span[ptile=1] affinity[core(2)]" -m "polus-c3-ib polus-c4-ib" OMP_NUM_THREADS=4 mpiexec ./prog_hyb -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 4
    bsub -J "launch mpi_omp" -n 4 -o logs/log.out.${N}.4.4 -e logs/log.err.${N}.4.4 -x -R "span[ptile=2] affinity[core(2)]" -m "polus-c3-ib polus-c4-ib" OMP_NUM_THREADS=4 mpiexec ./prog_hyb -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 4
    bsub -J "launch mpi_omp" -n 8 -o logs/log.out.${N}.8.4 -e logs/log.err.${N}.8.4 -x -R "span[ptile=4] affinity[core(2)]" -m "polus-c3-ib polus-c4-ib" OMP_NUM_THREADS=4 mpiexec ./prog_hyb -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 4

done

done
