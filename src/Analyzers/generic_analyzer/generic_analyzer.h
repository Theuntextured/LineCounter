#pragma once

#include "AppFramework/code_analyzer.h"

/*
 * Fallback analyzer.
 * Will assume everything is code (except for whitespaces).
 * No comments.
 */
class generic_analyzer : public code_analyzer
{
public:
    _NODISCARD counters analyze_file(const std::vector<std::string>& file_contents) const override;

    static bool is_only_whitespace(const std::string& str);
};
