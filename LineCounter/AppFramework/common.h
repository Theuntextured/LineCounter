#pragma once

#define FORCEINLINE __forceinline
#include <string_view>

inline constexpr std::string_view g_white_spaces = " \t\n\r";

#define CHECK(x) { if (!(x)) __debugbreak(); }