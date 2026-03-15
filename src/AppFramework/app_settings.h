#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include "AppFramework/CLI11.hpp"

namespace fs = std::filesystem;

class app_settings
{
public:
    app_settings() = default;
    app_settings(int argc, char* argv[]);
    

    fs::path root_folder; //--root -r
    std::vector<std::string> subfolders_to_exclude; //--exclude -e
    std::vector<std::string> file_extensions_to_include; //--ext -x
    fs::path log_file_path;
};

extern app_settings g_settings;
