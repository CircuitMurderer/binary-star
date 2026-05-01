#include <string>

namespace tidal::utils {
    using std::string;

    struct ArgsConfig {
        string config_path;
    };

    struct LocalConfig {
        int role;
        int port;
        string address;

        string expression;
        string data_path;
        string save_path;
    };

    ArgsConfig parse_args_config(int argc, char **argv);
    LocalConfig parse_local_config(const string& path);
}