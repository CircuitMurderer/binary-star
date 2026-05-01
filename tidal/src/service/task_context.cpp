#include <chrono>
#include <mutex>
#include <tuple>
#include <utility>
#include "task_context.hpp"

namespace tidal::service {
    void TaskContext::update_progress(unsigned current, unsigned total, std::string msg) {
        {
            std::unique_lock lock(this->mtx);
            this->progress = {current, total == 0 ? this->progress.second : total};
            if (!msg.empty()) this->message = std::move(msg);
        }

        if (this->on_update) on_update();
    }

    std::tuple<StatusCode, ErrorCode, RunProgress, std::string, long> TaskContext::snapshot() const {
        std::unique_lock lock(this->mtx);
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(this->end_time - this->start_time).count();
        return std::make_tuple(
            this->run_status.load(),
            this->error_code.load(),
            this->progress,
            this->message,
            duration
        );
    }

    void TaskContext::tick() {
        std::unique_lock lock(this->mtx);
        this->start_time = std::chrono::steady_clock::now();
    }

    void TaskContext::tock() {
        std::unique_lock lock(this->mtx);
        this->end_time = std::chrono::steady_clock::now();
    }
}