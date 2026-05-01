#include "main.hpp"
#include <spdlog/spdlog.h>

using namespace tidal;

int main(int argc, char **argv) {
    auto argsConf = utils::parse_args_config(argc, argv);
    auto localConf = utils::parse_local_config(argsConf.config_path);
    spdlog::info("[concurrency] {}", std::thread::hardware_concurrency());
    
    engine::CalculateRunner runner(localConf.role, localConf.port, localConf.address);
    runner.run(localConf.expression, localConf.data_path, localConf.save_path);
    
    return 0;
}