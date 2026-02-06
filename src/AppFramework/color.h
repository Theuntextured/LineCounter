#pragma once
#include <imgui.h>

struct color {
    float r, g, b, a;

    constexpr color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
    constexpr color(float in_r, float in_g, float in_b, float in_a = 1.0f)
        : r(in_r), g(in_g), b(in_b), a(in_a) {}

    [[nodiscard]] ImU32 to_u32() const {
        return ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));
    }

    static const color Black;
    static const color White;
    static const color Red;
    static const color Green;
    static const color Blue;
    static const color Yellow;
    static const color Magenta;
    static const color Cyan;
    static const color Transparent;
};

inline constexpr color color::Black(0.0f, 0.0f, 0.0f);
inline constexpr color color::White(1.0f, 1.0f, 1.0f);
inline constexpr color color::Red(1.0f, 0.0f, 0.0f);
inline constexpr color color::Green(0.0f, 1.0f, 0.0f);
inline constexpr color color::Blue(0.0f, 0.0f, 1.0f);
inline constexpr color color::Yellow(1.0f, 1.0f, 0.0f);
inline constexpr color color::Magenta(1.0f, 0.0f, 1.0f);
inline constexpr color color::Cyan(0.0f, 1.0f, 1.0f);
inline constexpr color color::Transparent(0.0f, 0.0f, 0.0f, 0.0f);