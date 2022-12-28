rm exe

#make
clang++ -O3 -std=c++11 main.cpp -o exe

#rm -r build
rm -r diags

./exe -it 2000 -p 100 -d 100
