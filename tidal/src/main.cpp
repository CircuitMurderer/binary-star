#include "main.hpp"

using namespace tidal;

int main(int argc, char **argv) {
    auto argsConf = utils::parseArgsConfig(argc, argv);
    auto localConf = utils::parseLocalConfig(argsConf.configPath);
    
    engine::CalculateRunner runner(localConf.role, localConf.port, localConf.address);
    runner.run(localConf.expression, localConf.dataPath, localConf.savePath);
    
    return 0;
}