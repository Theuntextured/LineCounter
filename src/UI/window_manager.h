#pragma once
#include <imgui.h>
#include <vector>

#include "AppFramework/color.h"
#include "GLFW/glfw3.h"


class window_manager
{
    friend class line_counter_app;
public:
    window_manager(line_counter_app* in_app);
    ~window_manager();
    void tick(const float dt);

    bool is_open() const;
    ImVec2 get_size() const;
private:
    void setup_file_explorer();
    void setup_file_statistics();
    void setup_quad_tree();
    void render_progress_graph();

    color background_color_{.5, .55, .57};
    line_counter_app* app_ = nullptr;

    GLFWwindow* window_handle = nullptr;

    std::vector<double> timestamps_;
    std::vector<double> file_count_;
    std::vector<double> line_of_code_counts;
    std::vector<double> line_counts_;
    std::vector<double> comment_line_counts_;
    std::vector<double> character_counts_;
};
