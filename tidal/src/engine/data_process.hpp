#pragma once
#include <string>
#include <vector>
#include "types.hpp"

namespace tidal::engine {
    using std::vector;
    using std::string;

    enum class CSVFormat {
        WithHeader,
        PureValues,
    };

    vector<Number> loadDataFromCSV(const string& path);
    void saveDataToCSV(const string& path, const vector<Number>& data, CSVFormat format = CSVFormat::WithHeader);
}