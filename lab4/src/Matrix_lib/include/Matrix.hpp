#ifndef SRC_MATRIX_LIB_INCLUDE_MATRIX
#define SRC_MATRIX_LIB_INCLUDE_MATRIX

/*
 * 1. the code uses operator[] instead of operator(row,column). It is not good
 * because it is not encapsulating the class. go for some ideas here:
 * http://www.parashift.com/c++-faq-lite/operator-overloading.html#faq-13.10
 * 2. the code cannot be easily adapted for processing units like GPU.
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

  constexpr Matrix<T, row_container, col_container> &
  operator+=(const Matrix<T, row_container, col_container> &b) {
    if (b.size() != this->size()) {
      throw std::invalid_argument("Matrices do not have same size.");
    }
    for (typename Matrix<T, row_container,
                         col_container>::matrix_type::size_type i = 0;
         i < b.data__.size(); ++i) {
      for (typename Matrix<T, row_container,
                           col_container>::vector_type::size_type ii = 0;
           ii < b.data__[0].size(); ++ii) {
        data__[i][ii] += b.data__[i][ii];
      }
    }

    return *this;
  }

  constexpr Matrix<T, row_container, col_container> &
  operator-=(const Matrix<T, row_container, col_container> &b) {
    if (b.size() != this->size()) {
      throw std::invalid_argument("Matrices do not have same size.");
    }
    for (typename Matrix<T, row_container,
                         col_container>::matrix_type::size_type i = 0;
         i < b.data__.size(); ++i) {
      for (typename Matrix<T, row_container,
                           col_container>::vector_type::size_type ii = 0;
           ii < b.data__[0].size(); ++ii) {
        data__[i][ii] -= b.data__[i][ii];
      }
    }
    return *this;
  }

  constexpr Matrix<T, row_container, col_container>
  operator+(const Matrix<T, row_container, col_container> &b) const {
    Matrix<T, row_container, col_container> tmp(*this);
    tmp += b;
    return tmp;
  }

  constexpr Matrix<T, row_container, col_container>
  operator-(const Matrix<T, row_container, col_container> &b) const {
    Matrix<T, row_container, col_container> tmp(*this);
    tmp -= b;
    return tmp;
  }

  constexpr Matrix<T, row_container, col_container> operator-() const {
    Matrix<T, row_container, col_container> tmp(*this);
    tmp -= (*this);
    tmp -= (*this);
    return tmp;
  }

  constexpr Matrix<T, row_container, col_container> transpose() const {
    Matrix<T, row_container, col_container> tmp;
    tmp.resize(data__[0].size(), data__.size());

    for (typename Matrix<T, row_container,
                         col_container>::matrix_type::size_type i = 0;
         i < data__.size(); ++i) {
      for (typename Matrix<T, row_container,
                           col_container>::vector_type::size_type ii = 0;
           ii < data__[0].size(); ++ii) {
        tmp[ii][i] = data__[i][ii];
      }
    }
    return tmp;
  }

  template <typename U = int>
  constexpr typename std::enable_if<std::is_integral<U>::value, U>::type
  rank() const {
    if constexpr (std::is_same<T, const char *>::value ||
                  std::is_same<T, std::string>::value) {
      // create garbage filled matrix with sizes of initial matrix
      Matrix<int64_t, row_container, col_container> mat;
      // those two if statements are needed for case of std::vector. for
      // std::array they should not be executed. actually, those if should be if
      // constexpr statements, but let's assume we allow other containers.
      if (data__.size() != mat.data__.size()) {
        mat.data__.resize(data__.size());
      }
      if (data__[0].size() != mat.data__[0].size()) {
        for (auto &row : mat.data__) {
          row.resize(data__[0].size());
        }
      }
      // fill the matrix with lengthes of initial matrix data
      if constexpr (std::is_same<T, std::string>::value) {
        for (U row = 0; row < data__.size(); row++) {
          for (U col = 0; col < data__[0].size(); col++) {
            mat.data__[row][col] = data__[row][col].size();
          }
        }
      } else {
        for (U row = 0; row < data__.size(); row++) {
          for (U col = 0; col < data__[0].size(); col++) {
            mat.data__[row][col] = strlen(data__[row][col]);
          }
        }
      }

    } else {
      auto mat = *this;
    }
    int rank = mat.data__[0].size();

    for (U row = 0; row < rank; row++) {
      // Before we visit current row 'row', we make
      // sure that mat[row][0],....mat[row][row-1]
      // are 0.

      // Diagonal element is not zero
      if (mat.data__[row][row]) {
        for (U col = 0; col < mat.data__.size(); col++) {
          if (col != row) {
            // This makes all entries of current
            // column as 0 except entry 'mat[row][row]'
            double mult = static_cast<double>(mat.data__[col][row]) /
                          mat.data__[row][row];
            for (U i = 0; i < rank; i++)
              mat.data__[col][i] -= mult * mat.data__[row][i];
          }
        }
      }

      // Diagonal element is already zero. Two cases
      // arise:
      // 1) If there is a row below it with non-zero
      //    entry, then swap this row with that row
      //    and process that row
      // 2) If all elements in current column below
      //    mat[r][row] are 0, then remove this column
      //    by swapping it with last column and
      //    reducing number of columns by 1.
      else {
        bool reduce = true;

        /* Find the non-zero element in current
            column  */
        for (U i = row + 1; i < mat.data__.size(); i++) {
          // Swap the row with non-zero element
          // with this row.
          if (mat.data__[i][row]) {
            // swap(mat.data__, row, i, rank);
            // Swap values in rows until the current rank.
            for (U ii = 0; ii < rank; ++ii) {
              int temp = mat.data__[row][ii];
              mat.data__[row][ii] = mat.data__[i][ii];
              mat.data__[i][ii] = temp;
            }

            reduce = false;
            break;
          }
        }

        // If we did not find any row with non-zero
        // element in current column, then all
        // values in this column are 0.
        if (reduce) {
          // Reduce number of columns
          rank--;

          // Copy the last column here
          for (U i = 0; i < mat.data__.size(); i++)
            mat.data__[i][row] = mat.data__[i][rank];
        }

        // Process this row again
        row--;
      }
    }
    return rank;
  }

  constexpr std::weak_ordering
  operator<=>(const Matrix<T, row_container, col_container> &rhs) const {
    std::weak_ordering tmp = std::weak_ordering::equivalent;
    for (typename Matrix<T, row_container,
                         col_container>::matrix_type::size_type i = 0;
         i < rhs.data__.size(); ++i) {
      for (typename Matrix<T, row_container,
                           col_container>::vector_type::size_type ii = 0;
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
  constexpr bool
  operator==(const Matrix<T, row_container, col_container> &rhs) const {
    return ((*this) <=> rhs) == 0;
  }
  constexpr bool
  operator!=(const Matrix<T, row_container, col_container> &rhs) const {
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

  constexpr friend std::ostream &
  operator<<(std::ostream &out,
             const Matrix<T, row_container, col_container> &m) {
    for (typename std::remove_reference<
             decltype(m)>::type::matrix_type::size_type i = 0;
         i < m.data__.size(); ++i) {
      for (typename std::remove_reference<
               decltype(m)>::type::vector_type::size_type ii = 0;
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
