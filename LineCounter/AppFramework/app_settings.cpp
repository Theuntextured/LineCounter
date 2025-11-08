#include "app_settings.h"
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static std::vector<std::string> split_by_comma(const std::string& input)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end;

    while ((end = input.find(',', start)) != std::string::npos)
    {
        result.push_back(input.substr(start, end - start));
        start = end + 1;
    }

    // Push the last segment (if any)
    if (start < input.size())
        result.push_back(input.substr(start));
    else if (input.back() == ',') // trailing comma case → add empty
        result.emplace_back();

    return result;
}


app_settings::app_settings(int argc, char* argv[])
{
    CLI::App app{"Line Counter"};
    argv = app.ensure_utf8(argv);

    root_folder = fs::current_path().string();
    app.add_option("-r,--root", root_folder, "Root folder to scan")->check(CLI::ExistingDirectory);
    std::string subfolders_to_exclude_params = ".git,.vs,Binaries,Intermediate,DerivedDataCache";
    app.add_option("-e,--exclude", subfolders_to_exclude_params, "Comma-separated list of subfolders to exclude");
    std::string file_extensions_to_include_params = "cpp,h,hpp,inl";
    app.add_option("-x,--ext", file_extensions_to_include_params, "Comma-separated list of file extensions to include");

    try { (app).parse(argc, argv); } catch(const CLI::ParseError &e) { (app).exit(e); return; }
    
    // --- Subfolders to exclude ---
    auto normalize_subfolder = [](std::string folder)
    {
        // Ensure preferred separators
        std::ranges::replace(folder, '\\', fs::path::preferred_separator);
        std::ranges::replace(folder, '/', fs::path::preferred_separator);

        // Ensure leading and trailing separator
        if (folder.front() != fs::path::preferred_separator)
            folder.insert(folder.begin(), fs::path::preferred_separator);
        if (folder.back() != fs::path::preferred_separator)
            folder.push_back(fs::path::preferred_separator);

        return folder;
    };
    
    // --- File extensions to include ---
    auto normalize_extension = [](std::string ext)
    {
        // remove leading '.'
        if (!ext.empty() && ext.front() == '.')
            ext.erase(ext.begin());
        return ext;
    };

    for (const auto& e : split_by_comma(subfolders_to_exclude_params))
        subfolders_to_exclude.push_back(normalize_subfolder(e));
    for (const auto& e : split_by_comma(file_extensions_to_include_params))
        file_extensions_to_include.push_back(normalize_extension(e));


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
