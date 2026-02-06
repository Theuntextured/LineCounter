#pragma once
#include <chrono>

#include "AppFramework/counters.h"
#include "UI/window_manager.h"

class line_counter_app
{
public:
    line_counter_app();

    void initialize();
    void run();
    void tick(const float dt);
    void cleanup();

    using Clock = std::chrono::high_resolution_clock;
    std::chrono::time_point<Clock> last_frame_time;

    bool running = true;
    class folder_node* root_node = nullptr;
    class tree_node* selected_node = nullptr;
    stat_type tree_stat_type = stat_type::total_lines_of_code;

    window_manager* app_window_manager;
    std::string hovered_path;
};
