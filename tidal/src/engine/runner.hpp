#include <memory>
#include <string>
#include <vector>
#include "calculator.hpp"
#include "equation_parser.hpp"
#include "types.hpp"

namespace tidal::engine {
    using std::string;
    using std::vector;
    using std::unique_ptr;

    class CalculateRunner {
    public:
        explicit CalculateRunner(int role, int port, string address);
        ~CalculateRunner() = default;

        void run(const string& expression, const string& dataPath, const string& savePath); // offline
        vector<Number> run(const string& expression, vector<Number>& dataSerial);           // online
    
    private:
        void lazyInit();
        vector<Number> process(vector<Number>& data, const string& expression);

        int role;
        int port;
        string address;

        unique_ptr<Calculator> calculator;
        unique_ptr<EquationParser> parser;
    };

}