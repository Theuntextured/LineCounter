#pragma once
#include <string>

#include "common.h"

enum class stat_type : uint8_t
{
    file_count,
    total_lines,
    total_lines_of_code,
    comment_only_lines,
    characters,
    num_types //MAX
};

inline const char* to_string(stat_type e)
{
    switch (e)
    {
    case stat_type::file_count: return "File Count";
    case stat_type::total_lines: return "Total Lines";
    case stat_type::total_lines_of_code: return "Total Lines of Code";
    case stat_type::comment_only_lines: return "Comment Only Lines";
    case stat_type::characters: return "Characters";
    default: return "Unknown";
    }
}

/*
 * Contains information about:
 * file count
 * total lines
 * total lines of code
 * comment only lines
 * total characters
 */
struct counters
{
    uint64_t files = 0;
    uint64_t lines = 0;
    uint64_t lines_of_code = 0;
    uint64_t comment_lines = 0;
    uint64_t characters = 0;

    uint64_t operator[](stat_type type) const;

    counters operator+(const counters& other) const;
    counters operator +=(const counters& other);

    [[nodiscard]] std::string to_string() const;

    bool is_valid = false;
    FORCEINLINE void invalidate() { is_valid = false; }
};
