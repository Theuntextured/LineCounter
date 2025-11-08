#pragma once
#include "AppFramework/code_analyzer.h"

class c_analyzer : public code_analyzer
{
public:
    _NODISCARD counters analyze_file(const std::vector<std::string>& file_contents) const override;
};
