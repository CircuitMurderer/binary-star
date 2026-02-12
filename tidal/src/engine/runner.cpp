#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "runner.hpp"
#include "calculator.hpp"
#include "data_process.hpp"
#include "equation_parser.hpp"
#include "types.hpp"

namespace tidal::engine {
    using std::string;
    using std::vector;
    using std::make_unique;

    CalculateRunner::CalculateRunner(int role, int port, string address) 
        : role(role), port(port), address(std::move(address)), calculator(nullptr), parser(nullptr) {}

    void CalculateRunner::lazyInit() {
        if (!this->calculator) this->calculator = make_unique<Calculator>(this->role, this->port, this->address);
        if (!this->parser) this->parser = make_unique<EquationParser>();
    }

    vector<Number> CalculateRunner::process(vector<Number>& data, const string& expression) {
        auto aliceData = this->calculator->input(data, this->role != ALICE);
        auto bobData = this->calculator->input(data, this->role != BOB);
        Context context = {{"A", aliceData}, {"B", bobData}};

        auto parsedRoot = this->parser->parse(expression);
        auto calculatedResult = parsedRoot->eval(context, *this->calculator);
        return this->calculator->output(calculatedResult);
    }

    void CalculateRunner::run(const string& expression, const string& dataPath, const string& savePath) {
        this->lazyInit();
        auto data = loadDataFromCSV(dataPath);
        auto result = this->process(data, expression);
        if (this->role == ALICE) saveDataToCSV(savePath, result);
    }

    vector<Number> CalculateRunner::run(const string& expression, vector<Number>& dataSerial) {
        this->lazyInit();
        return this->process(dataSerial, expression);
    }
}