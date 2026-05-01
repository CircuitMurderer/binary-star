#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "runner.hpp"
#include "calculator.hpp"
#include "utils/data_process.hpp"
#include "equation_parser.hpp"
#include "types.hpp"

namespace tidal::engine {
    using std::string;
    using std::vector;
    using std::make_unique;

    CalculateRunner::CalculateRunner(int role, int port, string address) 
        : role(role), port(port), address(std::move(address)), calculator(nullptr), parser(nullptr) {}

    void CalculateRunner::lazy_init() {
        if (!this->calculator) this->calculator = make_unique<Calculator>(this->role, this->port, this->address);
        if (!this->parser) this->parser = make_unique<EquationParser>();
    }

    vector<Number> CalculateRunner::process(vector<Number>& data, const string& expression) {
        auto alice_data = this->calculator->input(data, this->role != ALICE);
        auto bob_data = this->calculator->input(data, this->role != BOB);
        Context context = {{"A", alice_data}, {"B", bob_data}};

        auto parsed_root = this->parser->parse(expression);
        this->calculator->init_progress(parsed_root->size());
        auto calculated_result = parsed_root->eval(context, *this->calculator);
        return this->calculator->output(calculated_result);
    }

    void CalculateRunner::run(const string& expression, const string& data_path, const string& save_path) {
        this->lazy_init();
        auto data = loadDataFromCSV(data_path);
        auto result = this->process(data, expression);
        if (this->role == ALICE) saveDataToCSV(save_path, result);
    }

    vector<Number> CalculateRunner::run(const string& expression, vector<Number>& data_serial) {
        this->lazy_init();
        return this->process(data_serial, expression);
    }
}