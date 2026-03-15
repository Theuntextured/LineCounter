#include "csv.h"

bool csv::append_data_to_csv(const fs::path &file_path, counters data) {
    // Check if the file exists before we open it and potentially create it
    const bool file_exists = fs::exists(file_path);

    // std::ios::app ensures we append to the end and creates the file if missing
    std::ofstream file(file_path, std::ios::app);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open or create the file at " << file_path << "\n";
        return false;
    }

    // If the file did not exist, write the header row first
    if (!file_exists) {
        file << "timestamp,files,lines,lines_of_code,comment_lines,characters\n";
    }

    // Fetch the current system time
    const auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);

    // Write the timestamp and the uint64_t value separated by a comma
    file    << std::put_time(std::localtime(&current_time), "%Y-%m-%d %H:%M:%S") << ","
            << data.files           << ","
            << data.lines           << ","
            << data.lines_of_code   << ","
            << data.comment_lines   << ","
            << data.characters      << "\n";

    file.close();
    return true;
}

#include "csv.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

bool csv::load_progress_data(
    const fs::path &file_path,
    std::vector<double> &timestamps,
    std::vector<double> &files,
    std::vector<double> &lines,
    std::vector<double> &lines_of_code,
    std::vector<double> &comment_lines,
    std::vector<double> &characters)
{
    std::ifstream file(file_path);

    if (!file.is_open()) {
        std::cerr << "Failed to open the file: " << file_path << "\n";
        return false;
    }

    // Clear any existing data in all vectors
    timestamps.clear();
    files.clear();
    lines.clear();
    lines_of_code.clear();
    comment_lines.clear();
    characters.clear();

    std::string line;

    // Read the first line to skip the header
    if (std::getline(file, line)) {
        if (line.find("timestamp") == std::string::npos) {
            file.seekg(0);
        }
    }

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string time_str, files_str, lines_str, loc_str, comments_str, chars_str;

        // Extract all six columns
        if (std::getline(ss, time_str, ',') &&
            std::getline(ss, files_str, ',') &&
            std::getline(ss, lines_str, ',') &&
            std::getline(ss, loc_str, ',') &&
            std::getline(ss, comments_str, ',') &&
            std::getline(ss, chars_str)) {

            std::tm tm = {};
            std::stringstream time_ss(time_str);
            time_ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

            if (!time_ss.fail()) {
                try {
                    // Parse everything before pushing to ensure we do not end up with misaligned arrays if one fails
                    double epoch_time = static_cast<double>(std::mktime(&tm));
                    double f_val = std::stod(files_str);
                    double l_val = std::stod(lines_str);
                    double loc_val = std::stod(loc_str);
                    double com_val = std::stod(comments_str);
                    double char_val = std::stod(chars_str);

                    timestamps.push_back(epoch_time);
                    files.push_back(f_val);
                    lines.push_back(l_val);
                    lines_of_code.push_back(loc_val);
                    comment_lines.push_back(com_val);
                    characters.push_back(char_val);
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing a row's numeric values.\n";
                }
            }
        }
    }

    file.close();
    return true;
}
