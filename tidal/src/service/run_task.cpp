#include "run_task.hpp"
#include "service/task_context.hpp"
#include "service/thread_pool.hpp"
#include <exception>
#include <memory>
#include <string>
#include <utility>

namespace tidal::service {
    using tidal::engine::CalculateRunner;

    // template<typename F>
    // void submitTask(ThreadPool& pool, std::shared_ptr<TaskContext> ctx, F&& f) {
    //     ctx->runStatus = QUEUED;

    //     pool.enqueue([ctx, f = std::forward<F>(f)]() mutable {
    //         ctx->state = IN_RUNNING;
    //         try {
    //             ctx->runStatus = RUNNING;
    //             f();
    //             if (ctx->runStatus == RUNNING)
    //                 ctx->runStatus = COMPLETED;
    //         } catch (const std::exception& e) {
    //             ctx->runStatus = FAILED;
    //             ctx->updateProgress(0, 0, e.what());
    //         }
    //         ctx->state = FINISHED;
    //     });
    // }

    inline void submit_calculate_task(
        ThreadPool& pool,
        std::string task_id,
        int role,
        int port,
        std::string address,
        std::string expression,
        std::string data_path,
        std::string save_path
    ) {
        auto context = std::make_shared<TaskContext>(std::move(task_id));
        context->run_status = QUEUED;

        pool.enqueue([
            context,
            runner = CalculateRunner(
                role,
                port,
                std::move(address)
            ),
            expr = std::move(expression),
            d_path = std::move(data_path),
            s_path = std::move(save_path)
        ]() mutable {
            context->run_status = RUNNING;
            try {
                runner.run(expr, d_path, s_path);
                context->run_status = COMPLETED;
            } catch (const std::exception& e) {
                context->run_status = FAILED;
                context->update_progress(0, 0, e.what());
            }
        });
    }
}