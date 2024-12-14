#!/bin/bash

export OMP_SCHEDULE=static
export OMP_PROC_BIND=true
export OMP_DYNAMIC=false

for N in 128 256 512; do

#for i in 1 2 4 8 16; do
#    bsub -J "launch mpi" -n $i -o logs/log.out.${N}.${i} -eo logs/log.err.${N}.${i} -x -R "span[hosts=1]" -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
#    bsub -J "launch mpi" -n $i -o logs/log.out.${N}.${i} -eo logs/log.err.${N}.${i} -x -R "span[hosts=1]" -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
#    bsub -J "launch mpi" -n $i -o logs/log.out.${N}.${i} -eo logs/log.err.${N}.${i} -x -R "span[hosts=1]" -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
#done

#for i in 32; do
#    bsub -J "launch mpi" -n $i -o logs/log.out.${N}.${i} -eo logs/log.err.${N}.${i} -x -R "span[ptile=16]" -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
#    bsub -J "launch mpi" -n $i -o logs/log.out.${N}.${i} -eo logs/log.err.${N}.${i} -x -R "span[ptile=16]" -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
#    bsub -J "launch mpi" -n $i -o logs/log.out.${N}.${i} -eo logs/log.err.${N}.${i} -x -R "span[ptile=16]" -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
#done

for r in 1 2 3; do
    bsub -J "launch mpi" -n 1  -o logs/log.out.${N}.1  -eo logs/log.err.${N}.1  -x -R "span[hosts=1]"  -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
    bsub -J "launch mpi" -n 2  -o logs/log.out.${N}.2  -eo logs/log.err.${N}.2  -x -R "span[ptile=1]"  -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
    bsub -J "launch mpi" -n 4  -o logs/log.out.${N}.4  -eo logs/log.err.${N}.4  -x -R "span[ptile=2]"  -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
    bsub -J "launch mpi" -n 8  -o logs/log.out.${N}.8  -eo logs/log.err.${N}.8  -x -R "span[ptile=4]"  -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
    bsub -J "launch mpi" -n 16 -o logs/log.out.${N}.16 -eo logs/log.err.${N}.16 -x -R "span[ptile=8]"  -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1
    bsub -J "launch mpi" -n 32 -o logs/log.out.${N}.32 -eo logs/log.err.${N}.32 -x -R "span[ptile=16]" -m "polus-c3-ib polus-c4-ib" mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 1

    bsub -J "launch mpi_omp" -n 1 -o logs/log.out.${N}.1.4 -eo logs/log.err.${N}.1.4 -x -R "span[hosts=1] affinity[core(2)]" -m "polus-c3-ib polus-c4-ib" OMP_NUM_THREADS=4 mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 4
    bsub -J "launch mpi_omp" -n 2 -o logs/log.out.${N}.2.4 -eo logs/log.err.${N}.2.4 -x -R "span[ptile=1] affinity[core(2)]" -m "polus-c3-ib polus-c4-ib" OMP_NUM_THREADS=4 mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 4
    bsub -J "launch mpi_omp" -n 4 -o logs/log.out.${N}.4.4 -eo logs/log.err.${N}.4.4 -x -R "span[ptile=2] affinity[core(2)]" -m "polus-c3-ib polus-c4-ib" OMP_NUM_THREADS=4 mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 4
    bsub -J "launch mpi_omp" -n 8 -o logs/log.out.${N}.8.4 -eo logs/log.err.${N}.8.4 -x -R "span[ptile=4] affinity[core(2)]" -m "polus-c3-ib polus-c4-ib" OMP_NUM_THREADS=4 mpiexec ./prog_mpi -domain 1.0 -nodes $N -steps 20 -dt 1.e-5 -threads 4

done

done
