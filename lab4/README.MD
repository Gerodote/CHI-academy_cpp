how to?
1. get a c++ compiler that has `__cpp_constexpr 202103L`, which means it has support for `goto` in `constexpr` functions. ( why it's here? In code it's just showing the ability of using `if constexpr` instead of SFINAE sometimes)
2. `git clone https://github.com/Gerodote/CHI-academy_cpp.git` 
3. `cd ./CHI-academy_cpp/lab4`

then:

option a.
  1. get a `cmake` with version at least 3.20 
  2. `cmake -S. -B./build -DCMAKE_CXX_COMPILER=<name_of_your_cxx_compiler_or_path_to_it>` ( you don't have to write the -DCMA... . If you don't do that, the default compiler will be used )
  3. `cmake --build ./build`
  4. `./build/lab4`

option b. `compiler_name -std=c++2b main.cpp`. You can try `-std=c++23` or `-std=gnu++2b` or something else, depends on your compiler.
