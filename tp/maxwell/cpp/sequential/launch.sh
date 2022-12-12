#make
clang++ -O3 -std=c++11 main.cpp -o executable

#rm -r build
rm -r diags

./executable -nx 512 -ny 640 -it 2000 -p 100 -d 100
