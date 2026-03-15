#include "line_counter_app.h"
#include "AppFramework/app_settings.h"
#include "AppFramework/csv.h"
#include "Nodes/folder_node.h"


line_counter_app::line_counter_app()
{
    initialize();
    run();
    cleanup();   
}

void line_counter_app::initialize()
{
    root_node = new folder_node(g_settings.root_folder);
    selected_node = root_node;
    root_node->cleanup();
    std::cout << root_node->get_counters().to_string() << '\n';
    csv::append_data_to_csv(g_settings.log_file_path, root_node->get_counters());
    app_window_manager = new window_manager(this);
}

void line_counter_app::run()
{
    last_frame_time = Clock::now();
    while (running)
    {
        auto now = Clock::now();
        float dt = std::chrono::duration<float>(now - last_frame_time).count();
        last_frame_time = now;

        tick(dt);
    }
}

void line_counter_app::tick(const float dt)
{
    app_window_manager->tick(dt);
    running = app_window_manager->is_open();
}

void line_counter_app::cleanup()
{
    delete app_window_manager;
    delete root_node;   
}
