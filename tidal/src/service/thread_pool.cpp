#include <atomic>
#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <stop_token>
#include <type_traits>
#include <utility>
#include "spdlog/spdlog.h"
#include "thread_pool.hpp"

namespace tidal::service {
    ThreadPool::ThreadPool(size_t threads) {
        if (threads < 1) {
            spdlog::error("[thread_pool] thread pool must have at least 1 thread");
            throw std::invalid_argument("thread pool must have at least 1 thread");
        }

        for (size_t i = 0; i < threads; ++i) {
            this->workers.emplace_back([this](std::stop_token st) {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock lock(this->queue_mutex);
                        if (!this->condition.wait(lock, st, [this] { return !tasks.empty(); })) 
                            return;
                        
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                        ++this->active_tasks;
                    }

                    task();

                    if (--this->active_tasks == 0) {
                        std::scoped_lock lock(this->queue_mutex);
                        if (this->tasks.empty()) 
                            this->done_condition.notify_all();
                    }
                }
            });
        }
    }

    ThreadPool::~ThreadPool() {
        this->farewell();
    }

    void ThreadPool::farewell() {
        if (this->stop_flag.exchange(true, std::memory_order_acq_rel)) 
            return;

        for (auto& worker : this->workers) 
            worker.request_stop();

        for (auto& worker : this->workers) 
            if (worker.joinable())
                worker.join();
    }

    template<class F, class... Args>
    auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using ReturnType = std::invoke_result_t<F, Args...>;
        using TaskType = std::packaged_task<ReturnType()>;

        if (this->stop_flag.load(std::memory_order_acquire)) {
            spdlog::error("[thread_pool] enqueue on stopped thread pool");
            throw std::runtime_error("enqueue on stopped thread pool");
        }

        std::shared_ptr<TaskType> task = std::make_shared<TaskType>(
            [f = std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable {
                return f(std::move(args)...);
            }
        );

        std::future<ReturnType> result = task->get_future();
        {
            std::scoped_lock lock(this->queue_mutex);
            if (this->stop_flag.load(std::memory_order_acquire)) {
                spdlog::error("[thread_pool] enqueue on stopped thread pool (inner)");
                throw std::runtime_error("enqueue on stopped thread pool (inner)");
            }
            
            this->tasks.emplace([task]() { (*task)(); });
        }

        this->condition.notify_one();
        return result;
    }

    void ThreadPool::wait_done() {
        std::unique_lock lock(this->queue_mutex);
        this->done_condition.wait(lock, [this] {
            return this->tasks.empty() && this->active_tasks.load() == 0;
        });
    }
}