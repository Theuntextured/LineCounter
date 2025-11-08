#pragma once
#include <imgui.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>


class window_manager : public sf::RenderWindow
{
    friend class line_counter_app;
public:
    window_manager(line_counter_app* in_app);
    void tick(const sf::Time dt);
    
private:
    void process_events();

    void setup_file_explorer();
    void setup_file_statistics();
    void setup_quad_tree();

    sf::Color background_color_{127, 135, 138};
    line_counter_app* app_ = nullptr;
};
