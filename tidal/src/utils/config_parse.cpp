#include <stdexcept>
#include <utility>
#include "argh.h"
#include "sol/state.hpp"
#include "sol/table.hpp"
#include "spdlog/spdlog.h"
#include "config_parse.hpp"

namespace tidal::utils {
    using std::string;

    ArgsConfig parse_args_config(int argc, char **argv) {
        argh::parser cmdl;
        cmdl.add_params({"-c", "--config"});
        cmdl.parse(argc, argv);

        string config_path = cmdl({"-c", "--config"}, "config.lua").str();
        if (config_path.empty()) 
            spdlog::error("invalid configuration path provided");

        spdlog::info("[args_parse] starting with config: {}", config_path);
        return ArgsConfig { .config_path = std::move(config_path) };
    }

    LocalConfig parse_local_config(const string& path) {
        sol::state lua;
        lua.open_libraries(sol::lib::base);

        auto loaded_result = lua.script_file(path);
        if (!loaded_result.valid()) {
            sol::error err = loaded_result;
            spdlog::error("[config_parse] failed to load local config: {}", err.what());
            throw std::runtime_error("local config error");
        }

        sol::table loaded_config = lua["config"];
        if (!loaded_config.valid()) {
            spdlog::error("[config_parse] failed to load local config: missing 'config' key");
            throw std::runtime_error("local config error");
        }

        return LocalConfig {
            .role = loaded_config.get_or("role", 0),
            .port = loaded_config.get_or("port", 10240),
            .address = loaded_config.get_or("address", string("127.0.0.1")),

            .expression = loaded_config.get_or("expression", string("A+B")),
            .data_path = loaded_config.get_or("data_path", string("data.csv")),
            .save_path = loaded_config.get_or("save_path", string("result.csv")),
        };
    }
}