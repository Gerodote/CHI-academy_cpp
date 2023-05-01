#ifndef SRC_MATRIX_LIB_INCLUDE_MATRIX
#define SRC_MATRIX_LIB_INCLUDE_MATRIX

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

// using index_t = std::size_t;
// template <index_t N, index_t M, typename T = int>
// class Matrix;

template <typename T = int,
          template <class, typename...> class row_container = std::vector,
          template <class, typename...> class col_container = std::vector>
class Matrix {
  col_container<row_container<T>> data__;
  using matrix_type = typename std::remove_reference<decltype(data__)>::type;
  using vector_type = typename std::remove_reference<decltype(data__[0])>::type;

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

  std::tuple<typename matrix_type::size_type,
             typename vector_type::size_type> constexpr size() const {
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

  void resize(typename matrix_type::size_type rows,
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

  constexpr std::weak_ordering operator<=>(const Matrix<T> &rhs) const {
    std::weak_ordering tmp = std::weak_ordering::equivalent;
    for (typename Matrix<T>::matrix_type::size_type i = 0;
         i < rhs.data__.size(); ++i) {
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
                                            const Matrix<T, row_container, col_container> &m) {
    for (typename std::remove_reference<decltype(m)>::type::matrix_type::size_type i = 0; i < m.data__.size();
         ++i) {
      for (typename std::remove_reference<decltype(m)>::type::vector_type::size_type ii = 0;
           ii < m.data__[0].size(); ++ii) {
        // SFINAE. could be done using if constexpr.
        specific_printer<T>(out, m.data__[i][ii]);
      }
      out << '\n';
    }
    return out;
  }
};

#endif /* SRC_MATRIX_LIB_INCLUDE_MATRIX */
