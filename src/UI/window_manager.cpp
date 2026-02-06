#include "window_manager.h"
#include "imgui.h"
#include "App/line_counter_app.h"
#include "Nodes/folder_node.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

window_manager::window_manager(line_counter_app *in_app) : app_(in_app)
{
    if (!glfwInit()) {
        // Dungeons for the PC if this fails
        std::exit(1);
    }

    // Create window with graphics context
    window_handle = glfwCreateWindow(1280, 720, "Line Counter", nullptr, nullptr);
    if (window_handle == nullptr) {
        glfwTerminate();
        std::exit(1);
    }

    glfwMakeContextCurrent(window_handle);
    glfwSwapInterval(1); // Enable vsync

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window_handle, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

window_manager::~window_manager()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
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
    const ImVec2 window_size(get_size().x, get_size().y);
    const ImVec2 explorer_size = {window_size.x * .25f, window_size.y * .75f};

    const ImVec2 explorer_pos(window_size.x - explorer_size.x, 0.0f);

    ImGui::SetNextWindowPos(explorer_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(explorer_size, ImGuiCond_Always);
    
    ImGui::Begin("File Tree", nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    app_->hovered_path = (app_->root_node->get_path() / "").string();
    app_->root_node->setup_explorer(this, app_);
    ImGui::End();
}

void window_manager::setup_file_statistics()
{    
    const ImVec2 window_size(get_size().x, get_size().y);
    
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
    const ImVec2 QuadTreeSize(WindowSize.x * 0.75f, WindowSize.y);
    constexpr ImVec2 QuadTreePos(0.0f, 0.0f);

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

    if (app_ && app_->root_node)
        app_->selected_node->setup_treemap(this, app_, DrawList, CanvasMin, CanvasMax, true);

    ImGui::Dummy(CanvasSize);

    ImGui::End();
}

