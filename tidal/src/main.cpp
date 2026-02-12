#include "main.hpp"

int main(int argc, char **argv) {
    auto argsConf = tidal::utils::parseArgsConfig(argc, argv);
    auto localConf = tidal::utils::parseLocalConfig(argsConf.configPath);
    
    tidal::engine::CalculateRunner runner(localConf.role, localConf.port, localConf.address);
    runner.run(localConf.expression, localConf.dataPath, localConf.savePath);
    
    return 0;
}