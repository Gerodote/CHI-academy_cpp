#ifndef LIB_INCLUDE_ANALYZER
#define LIB_INCLUDE_ANALYZER

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <vector>

#include "priority_queue_with_messages_timestamps.hpp"

namespace fs = std::filesystem;

template <typename T, typename Container_t = std::vector<message_t<T>>, typename Comp = priority_less<T>>
class QueueAnalyzer
{
  public:
    QueueAnalyzer(const PriorityQueueWithMessagesTimestamps<T, Container_t, Comp> &queue) : queue_(queue)
    {
    }

    void analyze(const fs::path &file_path)
    {
        std::ofstream file(file_path, std::ios::out | std::ios::trunc);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file for writing.");
        }
        
        std::lock_guard<std::mutex> guard(queue_.m);

        // Get the current time
        const auto current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        file << "Current time: " << std::ctime(&current_time);

        // Get the size of the queue in KiB
        const auto queue_size_kib = (queue_.c.size() * sizeof(typename Container_t::value_type)) >> 10;
        file << "Queue size: " << queue_.c.size() << "\n";
        file << "Queue size (KiB): " << queue_size_kib << "\n";

        // Calculate the percentage of messages by their priority
        std::unordered_map<priority_type, uint64_t> count;
        count.reserve(queue_.c.size());
        for (const auto &message : queue_.c)
        {
            if (count.find(std::get<0>(message)) == count.end())
            {
                count[std::get<0>(message)] = 1;
            }
            else
            {
                ++count[std::get<0>(message)];
            };
        }
        file << "Priority distribution:\n";
        for (const auto &pair : count)
        {
            const auto percentage = (pair.second * 100.0) / queue_.c.size();
            file << "Priority " << pair.first << ": " << percentage << "%\n";
        }

        // Get the max time difference of messages
        if (!queue_.c.empty())
        {
            const auto minmax_it =
                std::minmax_element(queue_.c.begin(), queue_.c.end(), [](const auto &lhs, const auto &rhs) {
                    return std::get<1>(lhs) < std::get<1>(rhs);
                });
            const auto &oldest_message = *minmax_it.first;
            const auto &newest_message = *minmax_it.second;
            const auto max_time_diff = std::chrono::duration_cast<std::chrono::seconds>(std::get<1>(newest_message) -
                                                                                        std::get<1>(oldest_message))
                                           .count();
            file << "Max time difference of messages (s): " << max_time_diff << "\n";
        }
        file.close();
    }

  private:
    const PriorityQueueWithMessagesTimestamps<T, Container_t> &queue_;
};

#endif /* LIB_INCLUDE_ANALYZER */
