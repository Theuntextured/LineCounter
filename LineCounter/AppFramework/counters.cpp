#include "counters.h"
#include "format"

size_t counters::operator[](stat_type type) const
{
    switch (type)
    {
    case stat_type::file_count:
        return files;
    case stat_type::total_lines:
        return lines;
    case stat_type::total_lines_of_code:
        return lines_of_code;
    case stat_type::comment_only_lines:
        return comment_lines;
    case stat_type::characters:
        return characters;
    default:
        return 0;       
    }
}

counters counters::operator+(const counters& other) const
{
    return {
        .files = files + other.files,
        .lines = lines + other.lines,
        .lines_of_code = lines_of_code + other.lines_of_code,
        .comment_lines = comment_lines + other.comment_lines,
        .characters = characters + other.characters,
        .is_valid = is_valid && other.is_valid
    };
}

counters counters::operator+=(const counters& other)
{
    *this = *this + other;
    return *this;
}

std::string counters::to_string() const
{
    return std::format(
        "Files: {}\nLines: {}\nLines of code: {}\nComments: {}\nCharacters: {}",
        files, lines, lines_of_code, comment_lines, characters);
}
