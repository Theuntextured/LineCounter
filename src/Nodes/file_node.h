#pragma once
#include "AppFramework/tree_node.h"

class file_node : public tree_node
{
public:
    explicit file_node(const std::filesystem::path& in_path);
    
    [[nodiscard]] color get_fill_color() const override;
    ~file_node() override = default;
    counters get_counters() override;

    void setup_explorer(class window_manager* window_manager, class line_counter_app* app) override;
    void setup_treemap(
        class window_manager* window_manager, class line_counter_app* app,
        ImDrawList* draw_list,
        ImVec2 min, ImVec2 max,
        bool horizontal) override;
    
    static std::vector<std::string> read_file(const std::filesystem::path& file_path);
};
