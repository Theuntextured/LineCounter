#pragma once
#include "AppFramework/tree_node.h"

class folder_node : public tree_node
{
public:
    explicit folder_node(const std::filesystem::path& in_path);
    ~folder_node() override;
    
    counters get_counters() override;

    _NODISCARD color get_fill_color() const override;
    bool cleanup() override;

    void setup_explorer(class window_manager* window_manager, class line_counter_app* app) override;
    void setup_treemap(
        class window_manager* window_manager, class line_counter_app* app,
        ImDrawList* draw_list,
        ImVec2 min, ImVec2 max,
        bool horizontal) override;
private:
    std::vector<class file_node*> child_files_;
    std::vector<class folder_node*> child_folders_;

    static bool path_contains(const std::filesystem::path& path, std::string fragment);
    static bool file_has_extension(const std::filesystem::path& path, std::string extension);
};
