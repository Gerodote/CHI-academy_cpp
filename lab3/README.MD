How to check it:
1. `cd ./lab3`
2. `cmake -S. -B./build` ( without tests: add `-DBUILD_TESTING=OFF`)
3. `cmake --build ./build`
4. `cd build`
5. `ctest`
