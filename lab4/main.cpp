#include <array>
#include <vector>
#include <typeinfo>

#include "Matrix.hpp"

template <typename T> using arr_si_2 = std::array<T, 2>;

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

      std::sort(s3.begin(), s3.end());

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
      

      int res_test_18 = s4.rank();
      
      std::cout << res_test_18 << '\n';
      
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


      

      constexpr std::array<std::array<int, 2>, 2> const_arr{{{1, 2}, {2, 4}}};

      constexpr Matrix<int, arr_si_2, arr_si_2> const_m = const_arr;
      
      constexpr auto const_m2 = -const_m;

      constexpr int a_rank = const_m.rank();

      std::cout << a_rank << '\n';


      std::cout << const_m;

    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
}
