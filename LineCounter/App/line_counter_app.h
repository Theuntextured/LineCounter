#pragma once
#include "AppFramework/counters.h"
#include "UI/window_manager.h"

class line_counter_app
{
public:
    line_counter_app();

    void initialize();
    void run();
    void tick(const sf::Time dt);
    void cleanup();

    bool running = true;
    class folder_node* root_node = nullptr;
    class tree_node* selected_node = nullptr;
    stat_type tree_stat_type = stat_type::total_lines_of_code;

    window_manager* app_window_manager;
    std::string hovered_path;
};
