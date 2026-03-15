#pragma once
#include "window_manager.h"
#include "imgui.h"
#include "App/line_counter_app.h"
#include "Nodes/folder_node.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "AppFramework/csv.h"
#include "AppFramework/app_settings.h"

#ifndef APP_VERSION
#define APP_VERSION "v0.0.0-dev"
#endif

window_manager::window_manager(line_counter_app *in_app) : app_(in_app)
{
    if (!glfwInit()) {
        // Dungeons for the PC if this fails
        std::exit(1);
    }

    window_handle = glfwCreateWindow(1280, 720, "Line Counter " APP_VERSION, nullptr, nullptr);
    if (window_handle == nullptr) {
        glfwTerminate();
        std::exit(1);
    }

    glfwMakeContextCurrent(window_handle);
    glfwSwapInterval(1); // Enable vsync

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window_handle, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    csv::load_progress_data(g_settings.log_file_path,
        timestamps_,
        file_count_,
        line_counts_,
        line_of_code_counts,
        comment_line_counts_,
        character_counts_);
}

window_manager::~window_manager()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window_handle);
    glfwTerminate();
}

void window_manager::tick(const float dt)
{
    glfwPollEvents();

    if (glfwWindowShouldClose(window_handle)) {
        return;
    }

    // Start Frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    setup_file_explorer();
    setup_file_statistics();
    setup_quad_tree();
    render_progress_graph();
        
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window_handle, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window_handle);
}

bool window_manager::is_open() const {
    return !glfwWindowShouldClose(window_handle);
}

ImVec2 window_manager::get_size() const
{
    int w, h;
    glfwGetWindowSize(window_handle, &w, &h);
    return ImVec2(static_cast<float>(w), static_cast<float>(h));
}

void window_manager::setup_file_explorer()
{
    const ImVec2 window_size = get_size();
    // Shrunk height from .75f to .50f to make room for the graph
    const ImVec2 explorer_size = {window_size.x * 0.35f, window_size.y * 0.40f};
    const ImVec2 explorer_pos(window_size.x * 0.65f, 0.0f);

    ImGui::SetNextWindowPos(explorer_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(explorer_size, ImGuiCond_Always);

    ImGui::Begin("File Tree", nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    app_->hovered_path = (app_->root_node->get_path() / "").string();
    app_->root_node->setup_explorer(this, app_);
    ImGui::End();
}

void window_manager::render_progress_graph()
{
    const ImVec2 window_size = get_size();
    const ImVec2 graph_size = {window_size.x * 0.35f, window_size.y * 0.45f};
    const ImVec2 graph_pos(window_size.x * 0.65f, window_size.y * 0.40f);

    ImGui::SetNextWindowPos(graph_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(graph_size, ImGuiCond_Always);

    ImGui::Begin("Data Over Time", nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    // Passing ImVec2(-1, -1) forces the plot to fill the entire ImGui window nicely
    if (ImPlot::BeginPlot("Project Metrics Over Time", ImVec2(-1, -1))) {
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
        ImPlot::SetupAxis(ImAxis_Y1, "Count"); // Generic label since we have mixed data

        // Plotting each metric. Empty checks ensure we do not crash if data is misaligned.
        if (!file_count_.empty()) {
            ImPlot::PlotLine("Files",
                             timestamps_.data(), file_count_.data(), timestamps_.size());
        }

        if (!line_counts_.empty()) {
            ImPlot::PlotLine("Total Lines",
                             timestamps_.data(), line_counts_.data(), timestamps_.size());
        }

        if (!line_of_code_counts.empty()) {
            ImPlot::PlotLine("Lines of Code",
                             timestamps_.data(), line_of_code_counts.data(), timestamps_.size());
        }

        if (!comment_line_counts_.empty()) {
            ImPlot::PlotLine("Comments",
                             timestamps_.data(), comment_line_counts_.data(), timestamps_.size());
        }

        if (!character_counts_.empty()) {
            ImPlot::PlotLine("Characters",
                             timestamps_.data(), character_counts_.data(), timestamps_.size());
        }

        ImPlot::EndPlot();
    }

    ImGui::End();
}

void window_manager::setup_file_statistics()
{
    const ImVec2 window_size = get_size();

    // Fixed the bug here: changed window_size.x to window_size.y for the height parameter
    const ImVec2 stats_size(window_size.x * 0.35f, window_size.y * 0.15f);
    // Positioned at the 75% vertical mark, right below the new graph
    const ImVec2 stats_pos(window_size.x * 0.65f, window_size.y * 0.85f);

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
    ImGui::Text("Files: %u", counters.files);
    ImGui::Text("Lines: %u", counters.lines);
    ImGui::Text("Lines of code: %u", counters.lines_of_code);
    ImGui::Text("Comments: %u", counters.comment_lines);
    ImGui::Text("Characters: %u", counters.characters);

    ImGui::End();
}

void window_manager::setup_quad_tree()
{
    const ImVec2 WindowSize(get_size().x, get_size().y);
    const ImVec2 QuadTreeSize(WindowSize.x * 0.65f, WindowSize.y);
    constexpr ImVec2 QuadTreePos(0.0f, 0.0f);

    ImGui::SetNextWindowPos(QuadTreePos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(QuadTreeSize, ImGuiCond_Always);

    ImGui::Begin("Quad Tree", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar|
        ImGuiWindowFlags_NoScrollWithMouse);

    // --- Reserve a region inside the window as our "canvas" ---
    const ImVec2 CanvasSize = ImGui::GetContentRegionAvail();
    const ImVec2 CanvasMin  = ImGui::GetCursorScreenPos();
    const ImVec2 CanvasMax  = ImVec2(CanvasMin.x + CanvasSize.x, CanvasMin.y + CanvasSize.y);

    // Background
    ImDrawList* DrawList = ImGui::GetWindowDrawList();
    DrawList->AddRectFilled(CanvasMin, CanvasMax, IM_COL32(30, 30, 30, 255));
    DrawList->AddRect(CanvasMin, CanvasMax, IM_COL32(100, 100, 100, 255));

    if (app_ && app_->root_node)
        app_->selected_node->setup_treemap(this, app_, DrawList, CanvasMin, CanvasMax, true);

    ImGui::End();
}
