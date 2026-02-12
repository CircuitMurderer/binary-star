#include <stdexcept>
#include <utility>
#include "argh.h"
#include "sol/state.hpp"
#include "sol/table.hpp"
#include "spdlog/spdlog.h"
#include "config_parse.hpp"

namespace tidal::utils {
    using std::string;

    ArgsConfig parseArgsConfig(int argc, char **argv) {
        argh::parser cmdl;
        cmdl.add_params({"-c", "--config"});
        cmdl.parse(argc, argv);

        string configPath = cmdl({"-c", "--config"}, "config.lua").str();
        if (configPath.empty()) 
            spdlog::error("invalid configuration path provided");

        spdlog::info("[args_parse] starting with config: {}", configPath);
        return ArgsConfig { .configPath = std::move(configPath) };
    }

    LocalConfig parseLocalConfig(const string& path) {
        sol::state lua;
        lua.open_libraries(sol::lib::base);

        auto loadedResult = lua.script_file(path);
        if (!loadedResult.valid()) {
            sol::error err = loadedResult;
            spdlog::error("[config_parse] failed to load local config: {}", err.what());
            throw std::runtime_error("local config error");
        }

        sol::table loadedConfig = lua["config"];
        if (!loadedConfig.valid()) {
            spdlog::error("[config_parse] failed to load local config: missing 'config' key");
            throw std::runtime_error("local config error");
        }

        return LocalConfig {
            .role = loadedConfig.get_or("role", 0),
            .port = loadedConfig.get_or("port", 10240),
            .address = loadedConfig.get_or("address", string("127.0.0.1")),

            .expression = loadedConfig.get_or("expression", string("A+B")),
            .dataPath = loadedConfig.get_or("data_path", string("data.csv")),
            .savePath = loadedConfig.get_or("save_path", string("result.csv")),
        };
    }
}