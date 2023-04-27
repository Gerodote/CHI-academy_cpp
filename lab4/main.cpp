/*
 * WARNING : NEXT CODE IS A GARBAGE. DO NOT USE IN ANY PLACE.
 *
 * why the next code is a piece of garbage:
 * 1. it uses std::vector < std::vector > , which means more bad memory usage
 * and perhaps bad perfomance.
 * 2. the code uses operator[] instead of operator(row,column). It is not good
 * because it is not encapsulating the class. go for some ideas here:
 * http://www.parashift.com/c++-faq-lite/operator-overloading.html#faq-13.10
 * 3. the code cannot be easily adapted for processing units like GPU.
 * 4. no compile time stuff.
 * 5. It initializes at 0 0. + inefficiency.
 *
 * Q: ok, what use instead?
 * A: Any math matrix library e.g. Eigen, ArrayFire or something else.
 *
 * Q: ok, why the next code exists?
 * A: just for educational purposes.
 */

#include <algorithm>
#include <compare>
#include <cstring>
#include <iostream>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

// template <typename T>
// constexpr std::ostream& specific_printer(std::ostream & out, const T& val);

template <typename T>
constexpr std::ostream &specific_printer(
    std::ostream &out,
    typename std::enable_if<std::is_integral<T>::value, const T &>::type val) {
  out << val << ';';
  return out;
};

template <typename T>
constexpr std::ostream &specific_printer(
    std::ostream &out,
    typename std::enable_if<!std::is_integral<T>::value, const T &>::type val) {
  out << val << '|';
  return out;
};

template <typename T = int> class Matrix {
  std::vector<std::vector<T>> data__;
  using matrix_type = std::vector<std::vector<T>>;
  using vector_type = std::vector<T>;

public:
  constexpr Matrix() : data__({{}}){};
  constexpr Matrix(matrix_type arr) {
    for (auto it = std::next(arr.cbegin()); it != arr.cend(); ++it) {
      if ((*it).size() != (*arr.cbegin()).size()) {
        throw std::domain_error(
            "sizes of rows of inputted matrix are not equal.");
      }
      data__ = arr;
    }
  };
  constexpr std::tuple<typename matrix_type::size_type,
                       typename vector_type::size_type>
  size() const {
    return data__[0].size()
               ? std::make_tuple(data__.size(), data__[0].size())
               : std::make_tuple<typename matrix_type::size_type,
                                 typename vector_type::size_type>(0, 0);
  }

  constexpr vector_type
  operator[](typename matrix_type::size_type const &row_pos) const {
    return data__[row_pos];
  }

  constexpr vector_type &
  operator[](typename matrix_type::size_type const &row_pos) {
    return data__[row_pos];
  }

  constexpr void resize(typename matrix_type::size_type rows,
                        typename vector_type::size_type cols) {
    data__.resize(rows);
    for (typename matrix_type::size_type rows_i = 0; rows_i < rows; ++rows_i) {
      data__[rows_i].resize(cols);
    }
  }

  constexpr Matrix<T> &operator+=(const Matrix<T> &b) {
    if (b.size() != this->size()) {
      throw std::invalid_argument("Matrices do not have same size.");
    }
    for (typename Matrix<T>::matrix_type::size_type i = 0; i < b.data__.size();
         ++i) {
      for (typename Matrix<T>::vector_type::size_type ii = 0;
           ii < b.data__[0].size(); ++ii) {
        data__[i][ii] += b.data__[i][ii];
      }
    }

    return *this;
  }

  constexpr Matrix<T> &operator-=(const Matrix<T> &b) {
    if (b.size() != this->size()) {
      throw std::invalid_argument("Matrices do not have same size.");
    }
    for (typename Matrix<T>::matrix_type::size_type i = 0; i < b.data__.size();
         ++i) {
      for (typename Matrix<T>::vector_type::size_type ii = 0;
           ii < b.data__[0].size(); ++ii) {
        data__[i][ii] -= b.data__[i][ii];
      }
    }
    return *this;
  }

  constexpr Matrix<T> operator+(const Matrix<T> &b) const {
    Matrix<T> tmp(*this);
    tmp += b;
    return tmp;
  }

  constexpr Matrix<T> operator-(const Matrix<T> &b) const {
    Matrix<T> tmp(*this);
    tmp -= b;
    return tmp;
  }

  constexpr Matrix<T> operator-() const {
    Matrix<T> tmp;
    tmp.resize(data__.size(), data__[0].size());
    tmp -= (*this);
    return tmp;
  }

  constexpr Matrix<T> transpose() const {
    Matrix<T> tmp;
    tmp.resize(data__[0].size(), data__.size());

    for (typename Matrix<T>::matrix_type::size_type i = 0; i < data__.size();
         ++i) {
      for (typename Matrix<T>::vector_type::size_type ii = 0;
           ii < data__[0].size(); ++ii) {
        tmp[ii][i] = data__[i][ii];
      }
    }
    return tmp;
  }

  constexpr auto operator<=>(const Matrix<T> &rhs) const;
  constexpr bool operator==(const Matrix<T> &rhs) const {
    return ((*this) <=> rhs) == 0;
  }
  constexpr bool operator!=(const Matrix<T> &rhs) const {
    return !((*this) == rhs);
  }
  using iterator = typename matrix_type::iterator;

  constexpr iterator begin() { return data__.begin(); }
  constexpr iterator end() { return data__.end(); }

  using reverse_iterator = typename matrix_type::reverse_iterator;

  constexpr reverse_iterator rbegin() { return data__.rbegin(); }

  constexpr reverse_iterator rend() { return data__.rend(); }

  using const_iterator = typename matrix_type::const_iterator;

  constexpr const_iterator cbegin() { return data__.cbegin(); }

  constexpr const_iterator cend() { return data__.cend(); }

  using const_reverse_iterator = typename matrix_type::const_reverse_iterator;

  constexpr const_reverse_iterator crbegin() { return data__.crbegin(); }

  constexpr const_reverse_iterator crend() { return data__.crend(); }

  constexpr friend std::ostream &operator<<(std::ostream &out,
                                            const Matrix<T> &m) {
    for (typename Matrix<T>::matrix_type::size_type i = 0; i < m.data__.size();
         ++i) {
      for (typename Matrix<T>::vector_type::size_type ii = 0;
           ii < m.data__[0].size(); ++ii) {
        // SFINAE. could be done using if constexpr.
        specific_printer<T>(out, m.data__[i][ii]);
      }
      out << '\n';
    }
    return out;
  }
};

template <typename T>
constexpr auto Matrix<T>::operator<=>(const Matrix<T> &rhs) const {
  std::weak_ordering tmp = std::weak_ordering::equivalent;
  for (typename Matrix<T>::matrix_type::size_type i = 0; i < rhs.data__.size();
       ++i) {
    for (typename Matrix<T>::vector_type::size_type ii = 0;
         ii < rhs.data__[0].size(); ++ii) {
      std::weak_ordering cmp_val = std::weak_ordering::equivalent;
      if constexpr (std::is_same<T, std::string>::value) {
        cmp_val = ((data__[i][ii].size()) <=> (rhs.data__[i][ii].size()));

      } else if constexpr (std::is_same<T, const char *>::value) {
        cmp_val = (strlen(data__[i][ii]) <=> strlen(rhs.data__[i][ii]));
      } else {
        cmp_val = (data__[i][ii] <=> rhs.data__[i][ii]);
      };
      if (cmp_val != 0) {
        tmp = cmp_val;
        goto endloop;
      }
    }
  }
endloop:
  return tmp;
}

int main() {
  try {
    {
      Matrix<int> nothing;
      if (nothing.size() != std::make_tuple(0, 0)) {
        std::cerr << "idk, first test failed.\n"
                  << std::get<0>(nothing.size()) << ' '
                  << std::get<1>(nothing.size()) << '\n';
      }
    }
    {

      Matrix<int> something = std::vector<std::vector<int>>({{1, 2}, {3, 4}});
      if (something.size() != std::make_tuple(2, 2) || something[0][0] != 1 ||
          something[0][1] != 2 || something[1][0] != 3 ||
          something[1][1] != 4) {
        std::cerr << "second test failed.\n";
      }

      something[0][1] = 5;
      if (something[0][1] != 5) {
        std::cerr << "third test failed.\n";
      }

      something.resize(3, 3);
      if (something[0] != std::vector<int>({1, 5, 0}) ||
          something[1] != std::vector<int>({3, 4, 0}) ||
          something[2] != std::vector<int>({0, 0, 0})) {
        std::cerr << "fourth test failed";
      }

      something.resize(2, 2);
      if (something[0] != std::vector<int>({1, 5}) ||
          something[1] != std::vector<int>({3, 4})) {
        std::cerr << "fifth test failed";
      }

      std::cout << something << '\n';

      Matrix<int> s2 = std::vector<std::vector<int>>({{5, 1}, {3, 2}});
      something += s2;
      if (something[0] != std::vector<int>({6, 6}) ||
          something[1] != std::vector<int>({6, 6})) {
        std::cerr << "Test 6 failed. a result is:\n " << something;
      }

      try {
        something.resize(2, 3);
        something += s2;
      } catch (const std::invalid_argument &) {
        // nothing
      } catch (std::exception) {
        std::cerr << "test 7 failed.\n";
      }

      something.resize(2, 2);
      something += something;

      if (something[0] != std::vector<int>({12, 12}) ||
          something[1] != std::vector<int>({12, 12})) {
        std::cerr << "Test 8 failed. a result is:\n " << something;
      }

      something -= something;

      if (something[0] != std::vector<int>({0, 0}) ||
          something[1] != std::vector<int>({0, 0})) {
        std::cerr << "Test 9 failed. a result is:\n " << something;
      }

      something = something + s2;

      if (something[0] != std::vector<int>({5, 1}) ||
          something[1] != std::vector<int>({3, 2})) {
        std::cerr << "Test 10 failed. a result is:\n " << something;
      }

      something = s2 + s2 + s2 - something - something;

      if (something[0] != std::vector<int>({5, 1}) ||
          something[1] != std::vector<int>({3, 2})) {
        std::cerr << "Test 11 failed. a result is:\n " << something;
      }

      auto s3 = something.transpose();

      if (s3[0] != std::vector<int>({5, 3}) ||
          s3[1] != std::vector<int>({1, 2})) {
        std::cerr << "Test 12 failed. a result is:\n " << s3;
      }

      auto ic3 = s3.cbegin();

      if (*ic3 != std::vector<int>({5, 3}) ||
          (*std::next(ic3)) != std::vector<int>({1, 2}) ||
          (std::next(std::next(ic3))) != s3.cend()) {
        std::cerr << "Test 13 failed. a result is:\n " << (*ic3)[0]
                  << (*ic3)[1];
      }

      std::sort(s3.begin(), s3.end(), std::less<std::vector<int>>());

      if (s3[0] != std::vector<int>({1, 2}) ||
          s3[1] != std::vector<int>({5, 3})) {
        std::cerr << "Test 14 failed. a result is:\n " << s3;
      }

      auto s4 = -s3;

      if (s4[0] != std::vector<int>({-1, -2}) ||
          s4[1] != std::vector<int>({-5, -3})) {
        std::cerr << "Test 15 failed. a result is:\n " << s4;
      }

      s4 = -s4;

      if (s4 != s3) {
        std::cerr << "Test 16 failed. a result of comparisong: " << (s4 != s3);
      }
    }
    {
      Matrix<std::string> str_m =
          std::vector<std::vector<std::string>>({{"a", "b"}, {"c", "d"}});
      
      std::cout << str_m << '\n';

      Matrix<std::string> str_m2 =
          std::vector<std::vector<std::string>>({{"e", "f"}, {"g", "h"}});

      auto res_test_17 = (str_m == str_m2);
      if (!res_test_17) {
        std::cerr << "Test 17 failed. a result of comparisong: "
                  << (res_test_17);
      }


      // constexpr int i1 = 1;
      // constexpr int i2 = 2;
      // constexpr int i3 = 3;
      // constexpr int i4 = 4;
      // constexpr std::vector<int> c_v {i1,i2,i3,i4};

      // constexpr std::vector<std::vector<int> > c_ver {{1,2},{3,4}};
      // constexpr Matrix c_m = c_ver;
      // std::cout << c_m;

      // constexpr char const_str1[] = "e";
      // constexpr char const_str2[] = "f";
      // constexpr char const_str3[] = "g";
      // constexpr char const_str4[] = "h";
      // constexpr std::vector c_vec_str_1 {const_str1,const_str2};
      // constexpr std::vector c_vec_str_2 {const_str3, const_str4};
      // constexpr auto c_vec_str = std::vector({c_vec_str_1,c_vec_str_2});
      // constexpr Matrix c_str_m (c_vec_str);
      // constexpr auto c_str_m_size = c_str_m.size();
      // std::cout << std::get<0>(c_str_m_size) << ' ' <<
      // std::get<1>(c_str_m_size);
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
}
