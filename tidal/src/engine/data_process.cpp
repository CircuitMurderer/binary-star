#include <algorithm>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <vector>
#include "rapidcsv.h"
#include "spdlog/spdlog.h"
#include "data_process.hpp"

namespace tidal::engine {
    using std::vector;

    vector<Number> loadDataFromCSV(const string& path) {
        try {
            rapidcsv::Document doc(
                path, 
                rapidcsv::LabelParams(0, -1), 
                rapidcsv::SeparatorParams(',')
            );

            auto colNames = doc.GetColumnNames();
            if (std::ranges::find(colNames, "value") != colNames.end()) {
                return doc.GetColumn<Number>("value");
            } else {
                rapidcsv::Document docPure(
                    path,
                    rapidcsv::LabelParams(-1, -1),
                    rapidcsv::SeparatorParams(',')
                );

                size_t linesNumber = docPure.GetColumnCount();
                return docPure.GetColumn<Number>(linesNumber > 0 ? linesNumber - 1 : 0);
            }

        } catch (const std::exception& e) {
            spdlog::error("[data_process] failed to load CSV file {}: {}", path, e.what());
            return {};
        }
    }

    void saveDataToCSV(const string &path, const vector<Number> &data, CSVFormat format) {
        std::filesystem::path p(path);
        if (p.has_parent_path())
            std::filesystem::create_directories(p.parent_path());

        int columnNameIdx = (format == CSVFormat::WithHeader) ? 0 : -1;
        rapidcsv::Document doc(
            "", 
            rapidcsv::LabelParams(columnNameIdx, -1), 
            rapidcsv::SeparatorParams(',')
        );
        
        if (format == CSVFormat::WithHeader) {
            doc.SetColumnName(0, "number");
            doc.SetColumnName(1, "value");

            for (auto i = static_cast<int>(data.size() - 1); i >= 0; i--) {
                doc.SetCell<size_t>(0, i, i + 1);
                doc.SetCell<Number>(1, i, data[i]);
            }
        } else {
            for (auto i = static_cast<int>(data.size() - 1); i >= 0; i--) {
                doc.SetCell<Number>(0, i, data[i]);
            }
        }

        doc.Save(path);
        spdlog::info("[data_process] saved {} entries to {}", data.size(), path);
    }
}