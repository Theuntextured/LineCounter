#pragma once
#include <imgui.h>

#include "counters.h"
#include "SFML/Graphics.hpp"

class tree_node
{
public:
    explicit tree_node(const std::filesystem::path& in_path) : path_(in_path) {}
    virtual ~tree_node() = default;
    _NODISCARD virtual sf::Color get_fill_color() const = 0;
    virtual counters get_counters() = 0;
    virtual bool cleanup() { return true; }; //returns false if should be deleted

    virtual void setup_explorer(class window_manager* window_manager, class line_counter_app* app) = 0;
    virtual void setup_treemap(
        class window_manager* window_manager, class line_counter_app* app,
        ImDrawList* draw_list,
        ImVec2 min, ImVec2 max,
        bool horizontal = true) = 0;

    [[nodiscard]] const std::filesystem::path& get_path() const { return path_; }

protected:
    counters cached_counters_{};
    const std::filesystem::path path_;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
};
