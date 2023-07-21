
```bash
CXX=clang++ cmake ../ -DKokkos_ROOT=/Users/mathieu/Codes/kokkos/install/
```

```bash
srun --nodes=1 --time=00:30:00 -p gpu --gres=gpu:1 --pty /bin/bash
CXX=g++ cmake ../ -DKokkos_ROOT=${KOKKOS_HOME}/install/
make -j KOKKOS_DEVICES=Cuda KOKKOS_ARCH=Volta70
```