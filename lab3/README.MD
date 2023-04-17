How to check it:
1. `cd ./lab3`
2. `cmake -S. -B./build` ( without tests: add `-DBUILD_TESTING=OFF` and skip steps 4 & 5)
3. `cmake --build ./build`
4. `cd build`
5. `ctest`

In main CMakeLists.txt you can change c++ standard to c++17, if you wanna. If you want try more old standard ( or compiler without std::filesystem, like android's native c++17 compiler...), you shall need install boost's filessystem lib, then a little bit change headers and CMakeLists.txt
