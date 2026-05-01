#pragma once
#include <chrono>
#include <crow/websocket.h>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
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

        // ws
        void ws_send(crow::websocket::connection *conn, const std::string& msg);
        void ws_close(crow::websocket::connection *conn);

        void run(int port);

    private:
        unique_ptr<crow::App<RequestLogger, CORSMiddleware>> app;

        std::mutex ws_mtx;
        std::unordered_set<crow::websocket::connection*> ws_conns;

        void setup_ws();
    };
}
