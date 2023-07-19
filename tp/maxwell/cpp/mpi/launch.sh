rm exe

#make
mpic++ -O3 -std=c++11 main.cpp -o exe

#rm -r build
rm -r diags

mpirun -np 2 ./exe -it 2000 -p 100 -d 100