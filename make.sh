cmake -S . -B build
cmake --build build -- -j 8
cp build/main .
