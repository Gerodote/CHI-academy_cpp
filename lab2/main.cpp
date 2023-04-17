#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

class CustomClassThatCanConvertItselfToString;

class CustomClassThatCanConvertItselfToString {
  std::string data__;

public:
  CustomClassThatCanConvertItselfToString(
      std::string to_save = "IamBufferFromSstring")
      : data__(to_save) {}

  operator std::string() const { return data__; };
  operator const char *() const { return data__.c_str(); }
};

template <class T>
std::string specific_to_string(
    T const &smth,
    typename std::enable_if<std::is_same<T, bool>::value>::type * = 0) {
  std::ostringstream s;
  s << "B:" << std::boolalpha << smth;
  return s.str();
}

template <class T>
std::string specific_to_string(
    T const &smth,
    typename std::enable_if<!std::is_same<T, bool>::value &&
                            (std::is_integral<T>::value ||
                             std::is_floating_point<T>::value)>::type * = 0) {
  std::ostringstream s;
  s << "Ar:" << smth;
  return s.str();
}

template <class T>
std::string specific_to_string(
    T const &smth,
    typename std::enable_if<!std::is_integral<T>::value &&
                            (std::is_convertible<T, std::string>::value)>::type
        * = 0) {
  std::ostringstream s;
  s << "CTS:" << smth;
  return s.str();
}

struct CustomStringBuffer {
public:
  CustomStringBuffer(){};

  template <typename T> CustomStringBuffer &operator+=(T const &smth) {
    if ((buffer.size() != 0) && (*(std::next(buffer.crbegin())) != ' ')) {
      buffer += ' ';
    }
    buffer += specific_to_string(smth);
    return *this;
  }

  std::string buffer = "";
};

int main() {
  CustomClassThatCanConvertItselfToString Sstring;
  std::string string = "string_first";

  CustomStringBuffer SBuffer; // buffer = ""
  SBuffer += string;          // buffer = "CTS:string_first"
  SBuffer += 42;              // buffer = "CTS:string_first Ar:42"
  SBuffer += 4.2;             // buffer = "CTS:string_first Ar:42 Ar:4.2"
  SBuffer += -42424224244242; // buffer = "CTS:string_first Ar:42 Ar:4.2
                              // Ar:-42424224244242"
  SBuffer += Sstring;         // buffer = "CTS:string_first Ar:42 Ar:4.2
                              // Ar:-42424224244242 CTS:IamBufferFromSstring"
  SBuffer += false;           // buffer = "CTS:string_first Ar:42 Ar:4.2
                    // Ar:-42424224244242 CTS:IamBufferFromSstring B:false"

  std::cout << SBuffer.buffer << std::endl;
  //
  // CTS - Convert To String template overload
  // Ar  - Arithmetic template overload
  // B   - Boolean template overload

  return 0;
}
