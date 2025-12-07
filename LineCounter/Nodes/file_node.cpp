#include "file_node.h"
#include <fstream>
#include <imgui.h>
#include <iostream>

#include "App/line_counter_app.h"
#include "AppFramework/CLI11.hpp"
#include "AppFramework/code_analyzer.h"

file_node::file_node(const std::filesystem::path& in_path) : tree_node(in_path)
{
    std::cout << "File " << in_path << " processed with extension " << path_.extension().string() << std::endl;
}

sf::Color file_node::get_fill_color() const
{
    static std::unordered_map<std::string, sf::Color> color_table = {
        { "c", sf::Color::Red },
        { "cpp", sf::Color::Red },
        { "h", sf::Color::Blue },
        { "hpp", sf::Color::Blue },
        { "inl", sf::Color::Blue },
        { "cs", sf::Color::Yellow },
        { "java", sf::Color::Yellow },
        { "py", sf::Color::Green },
        { "usf", sf::Color::Magenta },
        { "hlsl", sf::Color::Magenta },
        { "glsl", sf::Color::Magenta },
    };
    auto ext = path_.extension().string();
    if (ext.starts_with(".")) ext.erase(ext.begin());
    if (const auto it = color_table.find(ext); it != color_table.end())
        return it->second;
    return sf::Color::White;  
}

counters file_node::get_counters()
{
    if (!cached_counters_.is_valid)
        cached_counters_ = analyzers::get_analyzer(
            path_.extension().string())
                    ->analyze_file(read_file(path_));
    return cached_counters_;   
}

void file_node::setup_explorer(class window_manager* window_manager, class line_counter_app* app)
{
    if (ImGui::TextLink(path_.filename().string().c_str()))
    {
#ifdef _WIN32
        // Windows
        ShellExecuteA(nullptr, "open", path_.string().c_str(), nullptr, nullptr, 0);
#elif __APPLE__
        // macOS
        std::string Command = "open \"" + FilePath.string() + "\"";
        std::system(Command.c_str());
#else
        // Linux / BSD / etc.
        std::string Command = "xdg-open \"" + FilePath.string() + "\"";
        std::system(Command.c_str());
#endif
    }

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip((path_.string() + "\n" + get_counters().to_string()).c_str());
}

void file_node::setup_treemap(class window_manager* window_manager, class line_counter_app* app,
    ImDrawList* draw_list, ImVec2 min, ImVec2 max, bool horizontal)
{
    if (min.x >= max.x || min.y >= max.y) return;
    bool hovered = (path_ / "").string().starts_with(app->hovered_path);
    
    sf::Color SfColor = get_fill_color();
    if (!hovered) SfColor.a *= 0.25;
    ImU32 FillColor = IM_COL32(SfColor.r, SfColor.g, SfColor.b, SfColor.a);
    ImU32 BorderColor = IM_COL32(80, 140, 220, 255);

    // Unique ID for ImGui item
    ImGui::PushID(path_.string().c_str());

    // Create an invisible hit zone matching the rectangle
    ImGui::SetCursorScreenPos(min);
    ImGui::InvisibleButton(("QuadRect##" + get_path().string()).c_str(), ImVec2(max.x - min.x, max.y - min.y));

    // Draw the actual rect behind the button area
    draw_list->AddRectFilled(min, max, FillColor, 4.0f);
    draw_list->AddRect(min, max, BorderColor, 4.0f, 0, 2.0f);

    // Tooltip logic
    if (ImGui::IsItemHovered())
    {
        std::string Tooltip = path_.string() + "\n" + get_counters().to_string();
        ImGui::SetTooltip("%s", Tooltip.c_str());
    }

    ImGui::PopID();
}


std::vector<std::string> file_node::read_file(const std::filesystem::path& file_path)
{
    std::vector<std::string> lines;
    std::ifstream file(file_path);

    if (!file.is_open())
        throw std::runtime_error("Failed to open file: " + file_path.string());

    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
            
        lines.push_back(std::move(line));
    }

    return lines;
}
