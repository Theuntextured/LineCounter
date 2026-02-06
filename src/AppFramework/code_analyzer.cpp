#include "code_analyzer.h"
#include <algorithm>

void analyzers::register_analyzer(const std::vector<std::string>& extensions, code_analyzer* analyzer)
{
    for (auto ext : extensions)
    {
        if (!ext.empty() && ext.front() == '.')
            ext.erase(ext.begin());
        std::ranges::transform(ext, ext.begin(), ::tolower);
        get_type_analyzers().insert_or_assign(ext, analyzer);
    }
}

void analyzers::register_default_analyzer(class code_analyzer* analyzer)
{
    default_analyzer_ = analyzer;
}

code_analyzer* analyzers::get_analyzer(const std::string& ext)
{
    std::string clean_ext = ext;
    if (!clean_ext.empty() && clean_ext.front() == '.')
        clean_ext.erase(clean_ext.begin());
    std::ranges::transform(clean_ext, clean_ext.begin(), ::tolower);

    if (const auto it = get_type_analyzers().find(clean_ext); it != get_type_analyzers().end())
        return it->second;

    return default_analyzer_;
}

std::map<std::string, class code_analyzer*>& analyzers::get_type_analyzers()
{
    static std::map<std::string, class code_analyzer*> type_analyzers;
    return type_analyzers;   
}

class code_analyzer* analyzers::default_analyzer_ = nullptr;
