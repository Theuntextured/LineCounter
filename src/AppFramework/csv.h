#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <cstdint>

#include "counters.h"

namespace fs = std::filesystem;
namespace csv {
    bool append_data_to_csv(const fs::path& file_path, counters data);
    bool load_progress_data(
        const fs::path &file_path,
        std::vector<double> &timestamps,
        std::vector<double> &files,
        std::vector<double> &lines,
        std::vector<double> &lines_of_code,
        std::vector<double> &comment_lines,
        std::vector<double> &characters);
}
