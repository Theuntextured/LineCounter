#include "c_analyzer.h"
#include <algorithm>
#include <string_view>

namespace
{
    static bool is_only_whitespace(const std::string& line)
    {
        return std::ranges::all_of(line, [](char c)
            { return g_white_spaces.find(c) != std::string_view::npos; });
    }

    static std::string trim_left(const std::string& s)
    {
        const auto pos = s.find_first_not_of(g_white_spaces);
        return (pos == std::string::npos) ? "" : s.substr(pos);
    }
}

counters c_analyzer::analyze_file(const std::vector<std::string>& file_contents) const
{
    counters out;
    out.is_valid = true;
    out.files = 1;
    out.lines = file_contents.size();

    bool in_block_comment = false;

    for (const auto& raw_line : file_contents)
    {
        out.characters += raw_line.size();

        std::string line = trim_left(raw_line);

        // --- Skip empty or whitespace-only lines
        if (is_only_whitespace(line))
            continue;

        // --- Handle comments
        if (in_block_comment)
        {
            out.comment_lines++;
            if (line.find("*/") != std::string::npos)
                in_block_comment = false;
            continue;
        }

        // Line starts a block comment
        const size_t block_start = line.find("/*");
        const size_t block_end = line.find("*/");

        // --- Full-line comment (single-line or block)
        if (line.starts_with("//"))
        {
            out.comment_lines++;
            continue;
        }
        else if (block_start != std::string::npos && (block_start == 0 || is_only_whitespace(line.substr(0, block_start))))
        {
            out.comment_lines++;
            if (block_end == std::string::npos)
                in_block_comment = true;
            continue;
        }

        // --- Lines that are code (may contain inline comments)
        out.lines_of_code++;
    }

    return out;
}

REGISTER_ANALYZER(c_analyzer, "c", "h", "cpp", "hpp", "inl", "cs", "hlsl", "usf", "ush", "java", "gradle"); //c-style means also other languages....
