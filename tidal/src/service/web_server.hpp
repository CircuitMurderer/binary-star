
#include <chrono>
#include <memory>
#include "crow/app.h"
#include "crow/http_response.h"
#include "crow/http_request.h"

namespace tidal::service {
    using std::unique_ptr;

    struct RequestLogger {
        struct context {
            std::chrono::time_point<std::chrono::high_resolution_clock> start;
        };

        void before_handle(crow::request& req, crow::response& res, context& ctx);
        void after_handle(crow::request& req, crow::response& res, context& ctx);
    };

    struct CORSMiddleware {
        struct context {};

        void before_handle(crow::request& req, crow::response& res, context& ctx);
        void after_handle(crow::request& req, crow::response& res, context& ctx);
    };

    class WebServer {
    public:
        explicit WebServer();
        ~WebServer() = default;

    private:
        unique_ptr<crow::App<RequestLogger, CORSMiddleware>> app;
        
    };
}
