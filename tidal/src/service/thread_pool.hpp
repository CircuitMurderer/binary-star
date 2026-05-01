#pragma once
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

namespace tidal::service {
    class ThreadPool {
    public:
        explicit ThreadPool(size_t threads = std::thread::hardware_concurrency());
        ~ThreadPool();

        void farewell();
        void wait_done();

        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

    private:
        std::atomic<bool> stop_flag{false};
        std::vector<std::jthread> workers;
        std::queue<std::function<void()>> tasks;
        
        std::mutex queue_mutex;
        std::condition_variable_any condition;
        std::condition_variable done_condition;

        std::atomic<uint32_t> active_tasks{0};
    };
}