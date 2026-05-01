#pragma once
#include <atomic>
#include <functional>
#include <shared_mutex>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <variant>

namespace tidal::service {
    using StatusCode = int;
    using ErrorCode = int;
    using RunProgress = std::pair<unsigned, unsigned>;

    constexpr StatusCode PLANNING = 1000;
    constexpr StatusCode QUEUED = 1001;
    constexpr StatusCode RUNNING = 1002;
    constexpr StatusCode COMPLETED = 1003;
    constexpr StatusCode FAILED = 1004;

    constexpr ErrorCode NO_ERROR = 0;
    constexpr ErrorCode RUNTIME_ERROR = 1;

    struct TaskContext {
        const std::string task_id;
        // std::atomic<ContextCode> state{NOT_STARTED};
        std::atomic<StatusCode> run_status{PLANNING};
        std::atomic<ErrorCode> error_code{NO_ERROR};
        // std::atomic<bool> manualCancel{false};

        mutable std::shared_mutex mtx;
        RunProgress progress{0, 0};
        std::string message{""};
        std::unordered_map<std::string, std::variant<bool, int, std::string>> extra_info;

        std::chrono::steady_clock::time_point start_time{std::chrono::steady_clock::now()};
        std::chrono::steady_clock::time_point end_time{std::chrono::steady_clock::now()};

        std::function<void()> on_update;

        void update_progress(unsigned current, unsigned total = 0, std::string msg = "");
        std::tuple<StatusCode, ErrorCode, RunProgress, std::string, long> snapshot() const;

        void tick();
        void tock();
    };
}