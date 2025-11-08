#include "line_counter_app.h"
#include <SFML/System.hpp>

#include "AppFramework/app_settings.h"
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
    app_window_manager = new window_manager(this);

}

void line_counter_app::run()
{
    sf::Clock clock;
    while (running)
        tick(clock.restart());
}

void line_counter_app::tick(const sf::Time dt)
{
    app_window_manager->tick(dt);
    running = app_window_manager->isOpen();
}

void line_counter_app::cleanup()
{
    delete app_window_manager;
    delete root_node;   
}
