#include "app_settings.h"
#include <iostream>

app_settings::app_settings(int argc, char* argv[])
{
    // --- Parse CLI arguments ---
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg.rfind("--", 0) == 0)
        {
            const auto eqPos = arg.find('=');
            if (eqPos != std::string::npos)
            {
                std::string key = arg.substr(2, eqPos - 2);
                std::string value = arg.substr(eqPos + 1);
                cli_params[key] = value;
            }
            else
            {
                // Flag-style arg (--something)
                cli_params[arg.substr(2)] = "true";
            }
        }
    }

    // --- Root folder ---
    if (cli_params.count("root"))
        root_folder = cli_params["root"];
    else
        root_folder = fs::current_path(); // Default to working directory

    // --- Subfolders to exclude ---
    if (cli_params.count("exclude"))
    {
        std::string list = cli_params["exclude"];
        size_t pos = 0;
        while ((pos = list.find(',')) != std::string::npos)
        {
            subfolders_to_exclude.push_back(list.substr(0, pos));
            list.erase(0, pos + 1);
        }
        if (!list.empty())
            subfolders_to_exclude.push_back(list);
    }
    else
    {
        // Common defaults
        subfolders_to_exclude = { ".git", ".vs", "Binaries", "Intermediate", "DerivedDataCache" };
    }

    // --- File extensions to include ---
    if (cli_params.count("ext"))
    {
        std::string list = cli_params["ext"];
        size_t pos = 0;
        while ((pos = list.find(',')) != std::string::npos)
        {
            std::string ext = list.substr(0, pos);
            if (ext[0] != '.')
                ext.insert(ext.begin(), '.');
            file_extensions_to_include.push_back(ext);
            list.erase(0, pos + 1);
        }
        if (!list.empty())
        {
            if (list[0] != '.')
                list.insert(list.begin(), '.');
            file_extensions_to_include.push_back(list);
        }
    }
    else
    {
        file_extensions_to_include = { ".cpp", ".h", ".hpp", ".inl" };
    }

    // --- Logging summary ---
    std::cout << "Root folder: " << root_folder << "\n";
    std::cout << "Excluded subfolders: ";
    for (auto& e : subfolders_to_exclude)
        std::cout << e << " ";
    std::cout << "\nIncluded extensions: ";
    for (auto& e : file_extensions_to_include)
        std::cout << e << " ";
    std::cout << "\n";
}

app_settings g_settings;