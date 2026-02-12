#include <string>

namespace tidal::utils {
    using std::string;

    struct ArgsConfig {
        string configPath;
    };

    struct LocalConfig {
        int role;
        int port;
        string address;

        string expression;
        string dataPath;
        string savePath;
    };

    ArgsConfig parseArgsConfig(int argc, char **argv);
    LocalConfig parseLocalConfig(const string& path);
}