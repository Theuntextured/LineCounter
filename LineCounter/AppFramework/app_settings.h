#pragma once
#include <map>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;  // NOLINT(misc-unused-alias-decls)

class app_settings
{
public:
    app_settings() = default;
    app_settings(int argc, char* argv[]);
    
    std::map<std::string, std::string> cli_params;

    fs::path root_folder; //--root
    std::vector<std::string> subfolders_to_exclude; //--exclude
    std::vector<std::string> file_extensions_to_include; //--ext
};

extern app_settings g_settings;