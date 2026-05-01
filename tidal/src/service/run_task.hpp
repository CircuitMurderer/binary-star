#pragma once
#include "engine/runner.hpp"
#include "service/task_context.hpp"
#include "service/thread_pool.hpp"
#include <memory>

namespace tidal::service {
    // template<typename F>
    // void submitTask(ThreadPool& pool, std::shared_ptr<TaskContext> ctx, F&& f);

    inline void submit_calculate_task(
        ThreadPool& pool, 
        std::string task_id, 
        int role, 
        int port, 
        std::string address,
        std::string expression,
        std::string data_path,
        std::string save_path
    );
}