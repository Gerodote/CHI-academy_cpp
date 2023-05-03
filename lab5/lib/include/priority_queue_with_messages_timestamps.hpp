#include <algorithm>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <ostream>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <boost/log/trivial.hpp>

class LimitReachedException : public std::exception {
public:
  [[nodiscard("exception")]] const char *what() const noexcept override {
    return "PriorityQueueWithMessagesTimestamps limit reached";
  }
};

using priority_type = std::int16_t;

template <typename T>
using message_t =
    std::tuple<priority_type, decltype(std::chrono::system_clock::now()), T>;

template <typename T> struct priority_less {
  constexpr bool operator()(const message_t<T> &lhs, const message_t<T> &rhs) {
    return std::get<0>(lhs) < std::get<0>(rhs);
  };
};

template <typename T, typename Container_t = std::vector<message_t<T>>,
          typename Comp = priority_less<T>>
class PriorityQueueWithMessagesTimestamps
    : protected std::priority_queue<message_t<T>, Container_t, Comp> {

  std::mutex m;
  uint64_t max_amount = 50;
public:
  using value_type = typename Container_t::value_type;
  using reference = typename Container_t::reference;
  using const_reference = typename Container_t::const_reference;
  using size_type = typename Container_t::size_type;
  using container_type = Container_t;

  PriorityQueueWithMessagesTimestamps() = default;

  PriorityQueueWithMessagesTimestamps(
      const PriorityQueueWithMessagesTimestamps &) = default;

  PriorityQueueWithMessagesTimestamps(PriorityQueueWithMessagesTimestamps &&) =
      default;

  PriorityQueueWithMessagesTimestamps &
  operator=(const PriorityQueueWithMessagesTimestamps &) = default;

  PriorityQueueWithMessagesTimestamps &
  operator=(PriorityQueueWithMessagesTimestamps &&) = default;

  bool is_empty() {
    std::lock_guard<std::mutex> guard(m);
    clear_expired();
    return this->c.empty();
  }

  size_t size() {
    std::lock_guard<std::mutex> guard(m);
    clear_expired();
    return this->c.size();
  }

  void pop() {
    std::lock_guard<std::mutex> guard(m);
    clear_expired();
    std::priority_queue<message_t<T>, Container_t, Comp>::pop();
  }

  void push(const message_t<T> &value) {
    std::lock_guard<std::mutex> guard(m);
    clear_expired();
    if (this->c.size() >= max_amount) {
      clear_expired();
      if (this->c.size() >= max_amount) {
        BOOST_LOG_TRIVIAL(info) << "Queue is full";
      }
    }
    std::priority_queue<message_t<T>, Container_t, Comp>::push(value);
    BOOST_LOG_TRIVIAL(info) << "Pushed an element" << value;
  }

  template <class... Args> void emplace(Args &&...args) {
    std::lock_guard<std::mutex> guard(m);
    clear_expired();
    if (this->c.size() >= max_amount) {
      BOOST_LOG_TRIVIAL(info) << "Queue is full";
    }
    std::priority_queue<message_t<T>, Container_t, Comp>::emplace(
        std::forward<Args>(args)...);
  }

  const message_t<T> &top() {
    std::lock_guard<std::mutex> guard(m);
    clear_expired();
    return this->c.front();
  }

  void clear_expired() {
    auto expired = [now = std::chrono::system_clock::now()](
                       const message_t<T> &m) { return std::get<1>(m) < now; };
    auto new_end = std::remove_if(this->c.begin(), this->c.end(), expired);
    this->c.erase(new_end, this->c.end());
    std::make_heap(this->c.begin(), this->c.end(), Comp{});
  }

  void set_max_amount(uint64_t max_amount) { this->max_amount = max_amount; }

  [[nodiscard]] uint64_t get_max_amount() const { return max_amount; }

  template <class, class, class> friend class QueueAnalyzer;
};

namespace fs = std::filesystem;

template <typename T, typename Container_t = std::vector<message_t<T>>,
          typename Comp = priority_less<T>>
class QueueAnalyzer {
public:
  QueueAnalyzer(
      const PriorityQueueWithMessagesTimestamps<T, Container_t, Comp> &queue)
      : queue_(queue) {}

  void analyze(const fs::path &file_path) {
    std::ofstream file(file_path, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
      throw std::runtime_error("Failed to open file for writing.");
    }

    // Get the current time
    const auto current_time =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    file << "Current time: " << std::ctime(&current_time);

    // Get the size of the queue in KiB
    const auto queue_size_kib =
        (queue_.c.size() * sizeof(typename Container_t::value_type)) >> 10;
    file << "Queue size: " << queue_.c.size() << "\n";
    file << "Queue size (KiB): " << queue_size_kib << "\n";

    // Calculate the percentage of messages by their priority
    std::unordered_map<priority_type, uint64_t> count;
    count.reserve(queue_.c.size());
    for (const auto &message : queue_.c) {
      if (count.find(std::get<0>(message)) == count.end()) {
        count[std::get<0>(message)] = 1;
      } else {
        ++count[std::get<0>(message)];
      };
    }
    file << "Priority distribution:\n";
    for (const auto &pair : count) {
      const auto percentage = (pair.second * 100.0) / queue_.c.size();
      file << "Priority " << pair.first << ": " << percentage << "%\n";
    }

    // Get the max time difference of messages
    if (!queue_.c.empty()) {
      const auto minmax_it =
          std::minmax_element(queue_.c.begin(), queue_.c.end(),
                              [](const auto &lhs, const auto &rhs) {
                                return std::get<1>(lhs) < std::get<1>(rhs);
                              });
      const auto &oldest_message = *minmax_it.first;
      const auto &newest_message = *minmax_it.second;
      const auto max_time_diff =
          std::chrono::duration_cast<std::chrono::seconds>(
              std::get<1>(newest_message) - std::get<1>(oldest_message))
              .count();
      file << "Max time difference of messages (s): " << max_time_diff << "\n";
    }
    file.close();
  }

private:
  const PriorityQueueWithMessagesTimestamps<T, Container_t> &queue_;
};
