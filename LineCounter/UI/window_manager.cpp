#include "window_manager.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include "App/line_counter_app.h"
#include "Nodes/folder_node.h"

window_manager::window_manager(line_counter_app* in_app)
    :app_(in_app)
{
    create(sf::VideoMode({800, 600}), "Line Counter", sf::Style::Default);
    ImGui::SFML::Init(*this);
}

void window_manager::tick(const sf::Time dt)
{
    process_events();
    
    ImGui::SFML::Update(*this, dt);

    setup_file_explorer();
    setup_file_statistics();
    setup_quad_tree();
        
    clear(background_color_);
    ImGui::SFML::Render(*this);
    display();
}

void window_manager::setup_file_explorer()
{
    const ImVec2 window_size(getSize().x, getSize().y);
    const ImVec2 explorer_size = {window_size.x * .25f, window_size.y * .75f};

    const ImVec2 explorer_pos(window_size.x - explorer_size.x, 0.0f);

    ImGui::SetNextWindowPos(explorer_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(explorer_size, ImGuiCond_Always);
    
    ImGui::Begin("File Tree", nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    app_->hovered_path = app_->root_node->get_path().string();
    app_->root_node->setup_explorer(this, app_);
    ImGui::End();
}

void window_manager::setup_file_statistics()
{    
    const ImVec2 window_size(getSize().x, getSize().y);
    
    const ImVec2 stats_size(window_size.x * .25f, window_size.x * .25f);
    const ImVec2 stats_pos(window_size.x * .75, window_size.y * .75f);

    ImGui::SetNextWindowPos(stats_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(stats_size, ImGuiCond_Always);

    ImGui::Begin("Statistics", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse);

    stat_type& current = app_->tree_stat_type;

    // Start a combo with the current selection label
    ImGui::Text("Statistic Type:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200.0f);
    if (ImGui::BeginCombo("##Statistic Type", to_string(current)))
    {
        // Iterate over all enum values (excluding the sentinel)
        for (int i = 0; i < static_cast<int>(stat_type::num_types); ++i)
        {
            stat_type Type = static_cast<stat_type>(i);
            const bool IsSelected = (current == Type);

            if (ImGui::Selectable(to_string(Type), IsSelected))
                current = Type;

            if (IsSelected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    ImGui::Text((app_->selected_node->get_path().string() + "\n").c_str());
    
    // Example stats (replace these with your real data)
    const auto counters = app_->selected_node->get_counters();
    ImGui::Text("Files: %d", counters.files);
    ImGui::Text("Lines: %d", 3332, counters.lines);
    ImGui::Text("Lines of code: %d", counters.lines_of_code);
    ImGui::Text("Comments: %d", counters.comment_lines);
    ImGui::Text("Characters: %d", counters.characters);

    ImGui::End();
}

void window_manager::setup_quad_tree()
{
    const ImVec2 WindowSize(getSize().x, getSize().y);
    const ImVec2 QuadTreeSize(WindowSize.x * 0.75f, WindowSize.y);
    const ImVec2 QuadTreePos(0.0f, 0.0f);

    ImGui::SetNextWindowPos(QuadTreePos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(QuadTreeSize, ImGuiCond_Always);

    ImGui::Begin("Quad Tree", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar);

    // --- Reserve a region inside the window as our "canvas" ---
    const ImVec2 CanvasSize = ImGui::GetContentRegionAvail();
    const ImVec2 CanvasMin  = ImGui::GetCursorScreenPos();
    const ImVec2 CanvasMax  = ImVec2(CanvasMin.x + CanvasSize.x, CanvasMin.y + CanvasSize.y);

    // Background
    ImDrawList* DrawList = ImGui::GetWindowDrawList();
    DrawList->AddRectFilled(CanvasMin, CanvasMax, IM_COL32(30, 30, 30, 255));
    DrawList->AddRect(CanvasMin, CanvasMax, IM_COL32(100, 100, 100, 255));

    // Optional: handle mouse interactions here later
    // e.g., zoom, pan, highlight hovered nodes, etc.

    // --- Draw your quadtree content ---
    if (app_ && app_->root_node)
        app_->selected_node->setup_treemap(this, app_, DrawList, CanvasMin, CanvasMax, true);

    // Reserve layout space for ImGui so the next widgets don’t overlap
    ImGui::Dummy(CanvasSize);

    ImGui::End();
}


void window_manager::process_events()
{
    while (const std::optional event = pollEvent())
    {
        ImGui::SFML::ProcessEvent(*this, event.value());
        if (event->is<sf::Event::Closed>())
            close();
        else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (key_pressed->scancode == sf::Keyboard::Scancode::Escape)
                close();
        }
    }


}
