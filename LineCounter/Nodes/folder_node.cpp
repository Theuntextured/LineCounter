#include "folder_node.h"
#include "file_node.h"
#include <filesystem>
#include <imgui.h>
#include <iostream>

#include "App/line_counter_app.h"
#include "AppFramework/app_settings.h"

namespace fs = std::filesystem;

folder_node::folder_node(const fs::path& in_path)
    : tree_node(in_path)
{
    try
    {
        if (!fs::exists(in_path) || !fs::is_directory(in_path))
            return;

        const auto& file_extensions_to_include = g_settings.file_extensions_to_include;
        const auto& subfolders_to_exclude = g_settings.subfolders_to_exclude;

        for (const auto& entry : fs::directory_iterator(in_path))
        {
            const fs::path& child_path = entry.path();

            try
            {
                if (entry.is_directory())
                {
                    // Skip excluded folders
                    bool excluded = false;
                    for (const auto& fragment : subfolders_to_exclude)
                    {
                        if (path_contains(child_path, fragment))
                        {
                            excluded = true;
                            break;
                        }
                    }
                    if (excluded)
                        continue;

                    // Recurse
                    auto* childFolder = new folder_node(child_path);
                    child_folders_.push_back(childFolder);
                }
                else if (entry.is_regular_file())
                {
                    // Check allowed extensions
                    bool allowed = false;
                    for (const auto& ext : file_extensions_to_include)
                    {
                        if (file_has_extension(child_path, ext))
                        {
                            allowed = true;
                            break;
                        }
                    }
                    if (!allowed)
                        continue;

                    // Add file node
                    auto* childFile = new file_node(child_path);
                    child_files_.push_back(childFile);
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error processing " << child_path << ": " << e.what() << '\n';
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error reading folder " << in_path << ": " << e.what() << '\n';
    }

    //std::cout << "Folder " << in_path << " processed\n";
}

folder_node::~folder_node()
{
    for (const tree_node* node : child_files_)
        delete node;
    for (const tree_node* node : child_folders_)
        delete node;
    child_files_.clear();
    child_folders_.clear();
}

counters folder_node::get_counters()
{
    if (cached_counters_.is_valid) return cached_counters_;
    cached_counters_.is_valid = true;
    for (tree_node* node : child_files_)
        cached_counters_ += node->get_counters();
    for (tree_node* node : child_folders_)
        cached_counters_ += node->get_counters();
    return cached_counters_;  
}

sf::Color folder_node::get_fill_color() const
{
    return sf::Color(100, 100, 100, 50);  
}

bool folder_node::cleanup()
{
    for (int i = static_cast<int>(child_folders_.size()) - 1; i >= 0; --i)
    {
        if (!child_folders_[i]->cleanup())
        {
            delete child_folders_[i];
            child_folders_.erase(child_folders_.begin() + i);
        }
    }
    
    return !child_files_.empty() || !child_folders_.empty();
}

void folder_node::setup_explorer(class window_manager* window_manager, class line_counter_app* app)
{
    const bool expanded = ImGui::TreeNode(path_.filename().string().c_str());


    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip((path_.string() + "\n" + get_counters().to_string()).c_str());
        app->hovered_path = (path_ / "").string();
    }

    ImGui::SameLine();
    if (ImGui::SmallButton(("Focus##" + path_.string()).c_str()))
    {
        app->selected_node = this;
    }
    
    
    if (expanded)
    {
        for (tree_node* node : child_folders_)
            node->setup_explorer(window_manager, app);
        for (tree_node* node : child_files_)
            node->setup_explorer(window_manager, app);
        ImGui::TreePop();   
    }
}

void folder_node::setup_treemap(class window_manager* window_manager, class line_counter_app* app,
    ImDrawList* draw_list, ImVec2 min, ImVec2 max, bool horizontal)
{
    if (min.x == max.x || min.y == max.y) return;
    //setup border
    {
        bool hovered = path_.string().starts_with(app->hovered_path);
        
        sf::Color SfColor = get_fill_color();
        if (!hovered) SfColor.a *= 0.25;
        ImU32 FillColor = IM_COL32(SfColor.r, SfColor.g, SfColor.b, SfColor.a);
        ImU32 BorderColor = IM_COL32(80, 80, 80, 255);

        constexpr float border_width = 1.0f;

        // Unique ID for ImGui item
        ImGui::PushID(path_.string().c_str());

        // Create an invisible hit zone matching the rectangle
        ImGui::SetCursorScreenPos(min);
        ImGui::InvisibleButton(("QuadRect##" + get_path().string()).c_str(), ImVec2(max.x - min.x, max.y - min.y));

        // Draw the actual rect behind the button area
        draw_list->AddRectFilled(min, max, FillColor, 4.0f);
        draw_list->AddRect(min, max, BorderColor, 4.0f, 0, border_width);

        // Tooltip logic
        if (ImGui::IsItemHovered())
        {
            std::string Tooltip = path_.string() + "\n" + get_counters().to_string();
            ImGui::SetTooltip("%s", Tooltip.c_str());
        }

        ImGui::PopID();

        min.x += border_width * 2;
        min.y += border_width * 2;
        max.x -= border_width * 2;
        max.y -= border_width * 2;   
    }
    
    float total = 0;
    float file_total = 0;
    for (tree_node* node : child_folders_)
        total += static_cast<float>(node->get_counters()[app->tree_stat_type]);
    for (tree_node* node : child_files_)
    {
        total += static_cast<float>(node->get_counters()[app->tree_stat_type]);
        file_total += static_cast<float>(node->get_counters()[app->tree_stat_type]);   
    }

    const auto slot_size = ImVec2(max.x - min.x, max.y - min.y);

    if (horizontal)
    {
        float current_percent_start = 0.0;
        for (tree_node* node : child_folders_)
        {
            const float node_percent = static_cast<float>(node->get_counters()[app->tree_stat_type]) / total;

            node->setup_treemap(window_manager, app, draw_list,
                ImVec2(min.x + slot_size.x * current_percent_start, min.y),
                ImVec2(min.x + slot_size.x * (current_percent_start + node_percent), max.y),
                !horizontal);
            
            current_percent_start += node_percent;
        }
        //these are drawn in the opposite orientation
        min = {min.x + slot_size.x * current_percent_start, min.y};
        current_percent_start = 0.0;
        
        for (tree_node* node : child_files_)
        {
            const float node_percent = static_cast<float>(node->get_counters()[app->tree_stat_type]) / file_total;

            node->setup_treemap(window_manager, app, draw_list,
                ImVec2(min.x, min.y + slot_size.y * current_percent_start),
                ImVec2(max.x, min.y + slot_size.y * (current_percent_start + node_percent)),
                !horizontal);
            
            current_percent_start += node_percent;
        }
    }
    else
    {
        float current_percent_start = 0.0;
        for (tree_node* node : child_folders_)
        {
            const float node_percent = static_cast<float>(node->get_counters()[app->tree_stat_type]) / total;

            node->setup_treemap(window_manager, app, draw_list,
                ImVec2(min.x, min.y + slot_size.y * current_percent_start),
                ImVec2(max.x, min.y + slot_size.y * (current_percent_start + node_percent)),
                !horizontal);
            
            current_percent_start += node_percent;
        }
        //these are drawn in the opposite orientation
        min = {min.x, min.y + slot_size.y * current_percent_start};
        current_percent_start = 0.0;
        
        for (tree_node* node : child_files_)
        {
            const float node_percent = static_cast<float>(node->get_counters()[app->tree_stat_type]) / file_total;

            node->setup_treemap(window_manager, app, draw_list,
                ImVec2(min.x + slot_size.x * current_percent_start, min.y),
                ImVec2(min.x + slot_size.x * (current_percent_start + node_percent), max.y),
                !horizontal);
            
            current_percent_start += node_percent;
        }
    }
}

bool folder_node::path_contains(const std::filesystem::path& path, std::string fragment)
{
    std::string path_str = path.string();

#ifdef _WIN32
    // Normalize both to lowercase for Windows
    std::ranges::transform(path_str, path_str.begin(), ::tolower);
    std::ranges::transform(fragment, fragment.begin(), ::tolower);
#endif

    return path_str.find(fragment) != std::string::npos;
}

bool folder_node::file_has_extension(const std::filesystem::path& path, std::string extension)
{
    if (!path.has_extension())
        return false;

    std::string path_ext = path.extension().string();

#ifdef _WIN32
    // Windows: case-insensitive
    std::ranges::transform(path_ext, path_ext.begin(), ::tolower);
    std::ranges::transform(extension, extension.begin(), ::tolower);
#endif

    // Normalize both (remove leading '.')
    if (!path_ext.empty() && path_ext.front() == '.')
        path_ext.erase(path_ext.begin());
    if (!extension.empty() && extension.front() == '.')
        extension.erase(extension.begin());

    return path_ext == extension;
}
