#ifndef TXT
#define TXT

#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

namespace l1 {

class Txt {
// i can write my own array but i guess i don't know now how to write
// it better for every specific system. There's a lot of research to do.
// i did long years ago same with new and delete[], but i told the problem
// about it here. 
    std::vector<std::string> lines__; 

public:
    Txt() : lines__() {}
    Txt(char const * filename) : lines__() {
        std::string line;
        std::ifstream file(filename);
        if (file.is_open()) {
            while (std::getline(file, line)) {
                lines__.emplace_back(line);
            }
            file.close();
        }
    }
    inline size_t size() const {
        return lines__.size();
    }
    Txt(const Txt& another) {
        this->lines__ = std::vector<std::string>(another.lines__);
    }
    inline Txt & operator=(Txt const& another) {
        if (this != &another) {
            lines__ = another.lines__;
        }
        return *this;
    }
    inline void swap (Txt & another_arr) {
        std::swap(this->lines__, another_arr.lines__);
    }
    Txt(Txt && rref) : Txt() {
        this->swap(rref);
    }
    inline Txt & operator=(Txt && rref) {
        if ( this != &rref ) {
            std::swap(lines__, rref.lines__);
        }
        return *this;
    }
    
};

}

#endif /* TXT */
