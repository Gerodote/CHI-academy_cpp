#ifndef LIB_INCLUDE_PRIORITY_QUEUE_WITH_MESSAGES_TIMESTAMPS
#define LIB_INCLUDE_PRIORITY_QUEUE_WITH_MESSAGES_TIMESTAMPS

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <mutex>
#include <ostream>
#include <queue>
#include <tuple>

#include <boost/log/trivial.hpp>

class LimitReachedException : public std::exception
{
  public:
    [[nodiscard("exception")]] const char *what() const noexcept override
    {
        return "PriorityQueueWithMessagesTimestamps limit reached";
    }
};

class AccessEmptyQueueException : public std::exception
{
  public:
    [[nodiscard("exception")]] const char *what() const noexcept override
    {
        return "AccessEmptyQueueException";
    }
};

using priority_type = std::int16_t;

template <typename T> using message_t = std::tuple<priority_type, decltype(std::chrono::system_clock::now()), T>;

template <typename T> struct priority_less
{
    constexpr bool operator()(const message_t<T> &lhs, const message_t<T> &rhs)
    {
        return std::get<0>(lhs) < std::get<0>(rhs);
    };
};

template <typename T, typename Container_t = std::vector<message_t<T>>, typename Comp = priority_less<T>>
class PriorityQueueWithMessagesTimestamps : protected std::priority_queue<message_t<T>, Container_t, Comp>
{

    mutable std::mutex m;
    uint64_t max_amount = 50;

  public:
    using value_type = typename Container_t::value_type;
    using reference = typename Container_t::reference;
    using const_reference = typename Container_t::const_reference;
    using size_type = typename Container_t::size_type;
    using container_type = Container_t;

    PriorityQueueWithMessagesTimestamps() = default;

    PriorityQueueWithMessagesTimestamps(const PriorityQueueWithMessagesTimestamps &) = default;

    PriorityQueueWithMessagesTimestamps(PriorityQueueWithMessagesTimestamps &&) = default;

    PriorityQueueWithMessagesTimestamps &operator=(const PriorityQueueWithMessagesTimestamps &) = default;

    PriorityQueueWithMessagesTimestamps &operator=(PriorityQueueWithMessagesTimestamps &&) = default;

    bool is_empty()
    {
        std::lock_guard<std::mutex> guard(m);
        clear_expired();
        return this->c.empty();
    }

    size_t size()
    {
        std::lock_guard<std::mutex> guard(m);
        clear_expired();
        return this->c.size();
    }

    void pop()
    {
        std::unique_lock<std::mutex> guard(m);
        clear_expired();
        if (this->c.empty())
        {
            guard.unlock();
            BOOST_LOG_TRIVIAL(error) << "Queue is empty";
            throw AccessEmptyQueueException();
        }
        std::priority_queue<message_t<T>, Container_t, Comp>::pop();
        BOOST_LOG_TRIVIAL(info) << "Popped an element";
    }

    void push(const message_t<T> &value)
    {
        std::unique_lock<std::mutex> guard(m);
        clear_expired();
        if (this->c.size() >= max_amount)
        {
            clear_expired();
            if (this->c.size() >= max_amount)
            {
                guard.unlock();
                BOOST_LOG_TRIVIAL(error) << "Queue is full";
                throw LimitReachedException();
            }
        }
        std::priority_queue<message_t<T>, Container_t, Comp>::push(value);
        guard.unlock();
        auto smth = std::chrono::system_clock::to_time_t(std::get<1>(value));
        BOOST_LOG_TRIVIAL(info) << "Pushed an element "
                                << " priority: " << std::setw(5) << std::get<0>(value)
                                << " exp.time: " << std::put_time(std::localtime(&smth), "%F %T")
                                << " value: " << std::left << std::setw(8) << std::get<2>(value);
    }

    template <class... Args> void emplace(Args &&...args)
    {
        std::unique_lock<std::mutex> guard(m);
        clear_expired();
        if (this->c.size() >= max_amount)
        {
            guard.unlock();
            BOOST_LOG_TRIVIAL(error) << "Queue is full";
            throw LimitReachedException();
        }
        std::priority_queue<message_t<T>, Container_t, Comp>::emplace(std::forward<Args>(args)...);
    }

    const message_t<T> &top()
    {
        std::unique_lock<std::mutex> guard(m);
        clear_expired();
        if (this->c.empty())
        {   
            guard.unlock();
            BOOST_LOG_TRIVIAL(error) << "Queue is empty";
            throw AccessEmptyQueueException();
        }
        return this->c.front();
    }

    void clear_expired() noexcept
    {
        auto expired = [now = std::chrono::system_clock::now()](const message_t<T> &m) { return std::get<1>(m) < now; };
        auto new_end = std::remove_if(this->c.begin(), this->c.end(), expired);
        this->c.erase(new_end, this->c.end());
        std::make_heap(this->c.begin(), this->c.end(), Comp{});
    }

    void set_max_amount(uint64_t max_amount)
    {
        this->max_amount = max_amount;
    }

    [[nodiscard]] uint64_t get_max_amount() const
    {
        return max_amount;
    }

    template <class, class, class> friend class QueueAnalyzer;
};

#endif // LIB_INCLUDE_PRIORITY_QUEUE_WITH_MESSAGES_TIMESTAMPS