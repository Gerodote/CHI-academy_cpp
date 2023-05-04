# CHI-academy_cpp

It's a code I made during pre-intern course of C++ in an academy of CHI company.
There was 5 labs.
If you are lurking for a useful code, you can try check 5th lab or 4th. those aren't that bad, actually.

the tasks were:

1. write a stupid class and check it perfomance. No interesting stuff.
2. play with template specializations and SFINAE.
3. write a priority queue with messages that have priority, an expire time and a data. Also write a friend class  that analyzes the priority queue and write the analysis to a given filepath
4. write a matrix library that can be constexpr, have +-, can be compared, can be calculated rank, can be printed in different ways depending on internal type of the matrix. if internal type is a string type, then matrix can be printed only using separator | . if there's needed an arithmetic stuff, then if there's a string type, calculate it length. Because of strange requirement to make a test in main.cpp, it's done without any test framework. No external lib used.
5. rewrite lab3 in threadsafe manner with logging and write a really specific test. Here's used boost::log library, so the code have the boost license.
