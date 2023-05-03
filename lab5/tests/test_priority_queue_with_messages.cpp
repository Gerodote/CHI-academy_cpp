#include <chrono>
#include <string>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "priority_queue_with_messages_timestamps.hpp"

TEST_CASE("PriorityQueueWithMessagesTimestamps test") {
  PriorityQueueWithMessagesTimestamps<int> queue;

  SUBCASE("test empty() method") {
    CHECK(queue.empty() == true);

    queue.emplace(
        1, std::chrono::system_clock::now() + std::chrono::minutes(10), 2);
    CHECK(queue.empty() == false);

    queue.clear_expired();
    CHECK(queue.empty() == false);
  }

  SUBCASE("test size() method") {
    CHECK(queue.size() == 0);

    queue.emplace(
        1, std::chrono::system_clock::now() + std::chrono::minutes(10), 2);
    CHECK(queue.size() == 1);

    queue.pop();
    CHECK(queue.size() == 0);
  }

  SUBCASE("test push() method") {
    queue.push(std::make_tuple(
        1, std::chrono::system_clock::now() + std::chrono::minutes(10), 2));
    CHECK(queue.size() == 1);
    queue.set_max_amount(1);
    CHECK_THROWS_AS(
        queue.push(std::make_tuple(1, std::chrono::system_clock::now(), 2)),
        LimitReachedException);
  }

  SUBCASE("test pop() method") {
    auto time_point1 =
        std::chrono::system_clock::now() + std::chrono::minutes(10);
    auto time_point2 =
        std::chrono::system_clock::now() + std::chrono::minutes(5);
    queue.emplace(1, time_point1, 2);
    queue.emplace(2, time_point2, 3);
    queue.pop();
    CHECK(queue.top() == std::make_tuple(1, time_point1, 2));
  }

  SUBCASE("test top() method") {
    auto time_point1 =
        std::chrono::system_clock::now() + std::chrono::minutes(10);
    auto time_point2 =
        std::chrono::system_clock::now() + std::chrono::minutes(5);
    queue.emplace(1, time_point1, 2);
    queue.emplace(2, time_point2, 3);
    CHECK(queue.top() == std::make_tuple(2, time_point2, 3));
  }

  SUBCASE("test clear_expired() method") {
    auto now = std::chrono::system_clock::now();
    queue.emplace(1, now - std::chrono::seconds(10), 2);
    queue.emplace(2, now - std::chrono::seconds(5), 3);
    queue.emplace(3, now + std::chrono::seconds(5), 4);
    queue.emplace(4, now + std::chrono::seconds(10), 5);
    queue.clear_expired();
    CHECK(queue.top() == std::make_tuple(4, now + std::chrono::seconds(10), 5));
    CHECK(queue.size() == 2);
  }

  SUBCASE("sorting in right way") {
    auto now = std::chrono::system_clock::now();
    queue.emplace(4, now + std::chrono::seconds(5), 4);
    queue.emplace(4, now + std::chrono::seconds(10), 5);
    queue.clear_expired();
    CHECK(queue.top() == std::make_tuple(4, now + std::chrono::seconds(5), 4));
    CHECK(queue.size() == 2);
  }

  SUBCASE("test set_max_amount() and get_max_amount() methods") {
    queue.set_max_amount(100);
    CHECK(queue.get_max_amount() == 100);
  }
}

TEST_CASE("Analyzer of PriorityQueueWithMessagesTimestamps") {
  PriorityQueueWithMessagesTimestamps<std::string> queue;
  SUBCASE("basic check.") {
    auto now = std::chrono::system_clock::now();
    queue.emplace(4, now + std::chrono::seconds(5), "hello there");
    queue.emplace(5, now + std::chrono::seconds(10), "hello there_2");
    QueueAnalyzer<std::string> analyzer(queue);
    analyzer.analyze(fs::path("./analysis.txt"));
    // idk how to hard it would be to write a full test here... just check
    // amount of lines. If you wanna, you can see ./build/test/analysis.txt
    std::ifstream inFile("analysis.txt");
    CHECK(7 == std::count(std::istreambuf_iterator<char>(inFile),
               std::istreambuf_iterator<char>(), '\n'));
  }
}
