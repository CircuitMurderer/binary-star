#include <chrono>
#include <crow/app.h>
#include <crow/websocket.h>
#include <mutex>
#include "crow/common.h"
#include "spdlog/spdlog.h"
#include "web_server.hpp"

namespace tidal::service {
    void RequestLogger::before_handle(crow::request& req, crow::response& res, context& ctx) {
        ctx.start = std::chrono::high_resolution_clock::now();
    }

    void RequestLogger::after_handle(crow::request& req, crow::response& res, context& ctx) {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - ctx.start).count();

        spdlog::info(
            "[web_server] {} | {}μs | {} | {} {}", 
            res.code, 
            duration, 
            req.remote_ip_address, 
            crow::method_name(req.method), 
            req.url
        );
    }

    void CORSMiddleware::before_handle(crow::request& req, crow::response& res, context& ctx) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.add_header("Access-Control-Max-Age", "3600");

        if (req.method == crow::HTTPMethod::Options) {
            res.code = 204;
            res.end();
        }
    }

    void CORSMiddleware::after_handle(crow::request& req, crow::response& res, context& ctx) {}

    void WebServer::ws_send(crow::websocket::connection *conn, const std::string& msg) {
        std::lock_guard lock(this->ws_mtx);
        if (this->ws_conns.contains(conn)) conn->send_text(msg);
    }

    void WebServer::ws_close(crow::websocket::connection* conn) {
        std::lock_guard lock(this->ws_mtx);
        if (this->ws_conns.contains(conn)) conn->close();
    }

    void WebServer::setup_ws() {
        CROW_WEBSOCKET_ROUTE((*app), "/ws/compute")
            .onopen([this](crow::websocket::connection& conn) {
                std::lock_guard lock(this->ws_mtx);
                this->ws_conns.insert(&conn);
            })
            .onclose([this](crow::websocket::connection& conn) {
                std::lock_guard lock(this->ws_mtx);
                this->ws_conns.erase(&conn);
            });
    }
}