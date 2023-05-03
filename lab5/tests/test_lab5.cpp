#include <atomic>
#include <chrono>
#include <condition_variable>
#include <random>
#include <thread>

#include <boost/log/trivial.hpp>

#include "analyzer.hpp"
#include "priority_queue_with_messages_timestamps.hpp"

namespace fs = std::filesystem;

std::condition_variable cv;
std::mutex cv_m;
std::atomic<int> i{0};

PriorityQueueWithMessagesTimestamps<double> pq;

uint64_t counter = 0;

void rand_push_pop(std::stop_token stoken)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    std::bernoulli_distribution int_dis(0.95);
    std::uniform_int_distribution<priority_type> pri_dis(-5, 5);
    std::uniform_int_distribution<long> time_dis(500, 2000);
    std::uniform_int_distribution<long> exp_time_dis(6000, 20000);
    while (!stoken.stop_requested())
    {
        if (int_dis(gen))
        {
            try
            {
                pq.push(message_t<double>(
                    pri_dis(gen), std::chrono::system_clock::now() + std::chrono::milliseconds(exp_time_dis(gen)),
                    dis(gen)));
            }
            catch (LimitReachedException &)
            {
                // send to signal using conditional variable
                i = 1;
                cv.notify_all();
            }
        }
        else
        {
            try
            {
                pq.pop();
            }
            catch (AccessEmptyQueueException &)
            {
                // nothing...
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(time_dis(gen)));
    }
}

void pop_every_sec(std::stop_token stoken)
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (stoken.stop_requested())
        {
            return;
        }
        try
        {
            pq.pop();
        }
        catch (AccessEmptyQueueException &)
        {
            // nothing... 
        }
    }
}

void analyzer_caller(std::stop_token stoken)
{
    auto in_a_min = std::chrono::system_clock::now() + std::chrono::minutes(1);
    do
    {
        {
            auto smth = std::chrono::system_clock::to_time_t(in_a_min);
            BOOST_LOG_TRIVIAL(info) << "Will be analyzing at most at " << std::put_time(std::localtime(&smth), "%F %T");
            std::unique_lock<std::mutex> lk(cv_m);
            if (!cv.wait_until(lk, in_a_min, [&stoken]() { return (i == 1) || stoken.stop_requested(); }))
            {
                in_a_min = std::chrono::system_clock::now() + std::chrono::minutes(1);
            };
            if (stoken.stop_requested())
            {
                break;
            }
            BOOST_LOG_TRIVIAL(info) << "Analyzing...";
            try
            {
                QueueAnalyzer analyzer(pq);
                analyzer.analyze(fs::path("./analysis_" + std::to_string(counter++) + ".txt"));
            }
            catch (std::runtime_error &e)
            {
                BOOST_LOG_TRIVIAL(error) << e.what() << '\n';
            }
            i = 0;
        }
    } while (!stoken.stop_requested());
    return;
}

int main()
{

    pq.set_max_amount(15);

    // start 3 threads doing rand_push_pop
    std::jthread t1(rand_push_pop);
    std::jthread t2(rand_push_pop);
    std::jthread t3(rand_push_pop);

    // start a thread soing pop_every_sec
    std::jthread t4(pop_every_sec);

    // start a thread doing analyzer_caller
    std::jthread t5(analyzer_caller);

    // wait 5 minutes
    std::this_thread::sleep_for(std::chrono::minutes(5));

    // join all threads of thread.
    t1.request_stop();
    t1.join();
    t2.request_stop();
    t2.join();
    t3.request_stop();
    t3.join();
    t4.request_stop();
    t4.join();
    t5.request_stop();
    t5.join();

    BOOST_LOG_TRIVIAL(info) << "Amount of calls of analyzer is " << counter << '\n';

    return 0;
}