cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build  -- -j 8
cp build/main .
cp build/all_tests .
