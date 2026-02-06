#include "generic_analyzer.h"
#include <algorithm>

counters generic_analyzer::analyze_file(const std::vector<std::string>& file_contents) const
{
    counters out;
    out.is_valid = true;
    out.files = 1;
    out.lines = file_contents.size();
    for (const auto& line : file_contents)
    {
        if (!is_only_whitespace(line)) out.lines_of_code++;
        out.characters += line.size();
    }
    return out;
}

bool generic_analyzer::is_only_whitespace(const std::string& str)
{
    return std::ranges::all_of(str, [](const char c)
    {
        return g_white_spaces.find(c) != std::string::npos;
    });
}

REGISTER_DEFAULT_ANALYZER(generic_analyzer);