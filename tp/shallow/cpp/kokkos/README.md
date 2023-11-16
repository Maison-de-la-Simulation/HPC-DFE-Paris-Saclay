
## Compilation CPU with LLVM

```bash
CXX=clang++ cmake ../ -DKokkos_ROOT=/Users/mathieu/Codes/kokkos/install/
```

## Compilation GPU on Ruche with GCC

```bash
CXX=g++ cmake ../ -DKokkos_ROOT=${KOKKOS_HOME}/install/
make -j KOKKOS_DEVICES=Cuda KOKKOS_ARCH=Volta70
```

Interactive node:
```bash
srun --nodes=1 --time=00:30:00 -p gpu --gres=gpu:1 --pty /bin/bash
```