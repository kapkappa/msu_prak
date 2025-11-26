scl enable devtoolset-9 -- bash
source /polusfs/setenv/setup.OpenMPI
module load OpenMPI/4.0.2

make MACHINE=Polus
mkdir logs
./launch.sh
./get.sh

