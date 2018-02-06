#include "gui.hpp"

namespace io
{
    void gui::init()
    {
        // Save the default ImGui context.
        // At execution termination, restore the default context and Shutdown ImGui.
        // This is needed because for some reason ImGui needs a valid context on ImGui::Shutdown().
        static class ImGuiInit
        {
        public:
            ImGuiInit() : m_default_context(ImGui::GetCurrentContext()) {}
            ~ImGuiInit()
            {
                ImGui::SetCurrentContext(m_default_context);
                ImGui::Shutdown();
            }
        private:
            ImGuiContext * m_default_context;
        } imgui_init;

        _context = ImGui::CreateContext();
        ImGui::SetCurrentContext(_context);

        auto&& io = _context->IO;
        io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;                         // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
        io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
        io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
        io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
        io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
        io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
        io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
        io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
        io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
        io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
        apply_theme();

        io.UserData = this;
        io.RenderDrawListsFn = nullptr;

        io.ClipboardUserData = _window;
        io.SetClipboardTextFn = [](void* data, const char* text) { glfwSetClipboardString(reinterpret_cast<GLFWwindow*>(data), text); };
        io.GetClipboardTextFn = [](void* data) { return glfwGetClipboardString(reinterpret_cast<GLFWwindow*>(data)); };

        init_atlas();
    }

    void gui::new_frame()
    {
        _has_frame = true;
        ImGui::SetCurrentContext(_context);

        auto&& io = _context->IO;
        int w, h;
        int display_w, display_h;
        glfwGetWindowSize(_window, &w, &h);
        glfwGetFramebufferSize(_window, &display_w, &display_h);
        io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
        io.DisplayFramebufferScale = ImVec2(w > 0 ? (static_cast<float>(display_w) / w) : 0, h > 0 ? (static_cast<float>(display_h) / h) : 0);

        io.DeltaTime = static_cast<float>(glfwGetTime() - _last_time);
        _last_time = glfwGetTime();

        if (glfwGetWindowAttrib(_window, GLFW_FOCUSED))
        {
            if (io.WantMoveMouse)
            {
                glfwSetCursorPos(_window, static_cast<double>(io.MousePos.x), static_cast<double>(io.MousePos.y));
            }
            else
            {
                double mouse_x, mouse_y;
                glfwGetCursorPos(_window, &mouse_x, &mouse_y);
                io.MousePos = ImVec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
            }
        }
        else
        {
            io.MousePos = ImVec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
        }

        for (int i = 0; i < 3; i++)
        {
            // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
            io.MouseDown[i] = _mouse_button_states[i] || glfwGetMouseButton(_window, i) != 0;
            _mouse_button_states[i] = false;
        }

        io.MouseWheel = _mouse_wheel_delta;
        _mouse_wheel_delta = 0;

        ImGui::NewFrame();
    }

    bool gui::key_action(const int key, const int action, [[maybe_unused]] int mods) const
    {
        auto&& io = _context->IO;
        if (action == GLFW_PRESS)
            io.KeysDown[key] = true;
        if (action == GLFW_RELEASE)
            io.KeysDown[key] = false;

        io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
        io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
        io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
        io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
        return io.WantCaptureKeyboard && io.WantTextInput;
    }

    bool gui::mouse_button_action(const int button, const int action, [[maybe_unused]] int mods) const
    {
        auto&& io = _context->IO;
        if (action == GLFW_PRESS && button >= 0 && button < 3)
            _mouse_button_states[button] = true;
        return io.WantCaptureMouse;
    }

    void gui::scrolled(const double delta_y) const
    {
        _mouse_wheel_delta += static_cast<float>(delta_y);
    }

    bool gui::char_input(const wchar_t character) const
    {
        auto&& io = _context->IO;
        if (character > 0 && character < 0x10000)
            io.AddInputCharacter(static_cast<unsigned short>(character));
        return io.WantCaptureKeyboard && io.WantTextInput;
    }

    bool gui::is_initialized() const
    {
        return _has_frame;
    }

    void gui::render()
    {
        ImGui::SetCurrentContext(_context);
        ImGui::Render();
        render_data(ImGui::GetDrawData());
    }

    void gui::render_data(ImDrawData* draw_data)
    {
        auto&& io = _context->IO;
        const int fb_width = static_cast<int>(io.DisplaySize.x * io.DisplayFramebufferScale.x);
        const int fb_height = static_cast<int>(io.DisplaySize.y * io.DisplayFramebufferScale.y);
        if (fb_width == 0 || fb_height == 0)
            return;
        draw_data->ScaleClipRects(io.DisplayFramebufferScale);

        const auto vertex_count = draw_data->TotalVtxCount;
        const auto index_count = draw_data->TotalIdxCount;
        if (vertex_count * index_count == 0)
            return;

        pre_render(draw_data);

        size_t vtx_buffer_offset = 0;
        size_t idx_buffer_offset = 0;
        for (auto n = 0; n < draw_data->CmdListsCount; n++)
        {
            const auto cmd_list = draw_data->CmdLists[n];

            for (auto cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
            {
                const auto& pcmd = cmd_list->CmdBuffer[cmd_i];
                if (pcmd.UserCallback)
                {
                    pcmd.UserCallback(cmd_list, &pcmd);
                }
                else
                {
                    mid_render(pcmd, static_cast<int>(idx_buffer_offset), static_cast<int>(vtx_buffer_offset));
                }
                idx_buffer_offset += pcmd.ElemCount;
            }
            vtx_buffer_offset += cmd_list->VtxBuffer.Size;
        }

        post_render();
    }

    void gui::apply_theme() const
    {
        auto atlas = _context->IO.Fonts;
        // Default Font
        atlas->AddFontFromFileTTF("../res/ui/fonts/Ruda-Bold.ttf", 12);
        atlas->AddFontFromFileTTF("../res/ui/fonts/Ruda-Bold.ttf", 10);
        atlas->AddFontFromFileTTF("../res/ui/fonts/Ruda-Bold.ttf", 14);
        atlas->AddFontFromFileTTF("../res/ui/fonts/Ruda-Bold.ttf", 18);

        atlas->Build();

        _context->IO.FontDefault = atlas->Fonts[font_default];

        auto&& style = &_context->Style;
        style->WindowPadding = ImVec2(15, 15);
        style->WindowRounding = 5.0f;
        style->FramePadding = ImVec2(5, 5);
        style->FrameRounding = 4.0f;
        style->ItemSpacing = ImVec2(12, 8);
        style->ItemInnerSpacing = ImVec2(8, 6);
        style->IndentSpacing = 25.0f;
        style->ScrollbarSize = 15.0f;
        style->ScrollbarRounding = 9.0f;
        style->GrabMinSize = 5.0f;
        style->GrabRounding = 3.0f;

        style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
        style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        style->Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);//0.80f, 0.80f, 0.83f, 0.88f
        style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
        style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
        style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
        style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
        style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
        style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
        style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
        style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

    }

    gui::~gui()
    {
        if (_context)
            ImGui::DestroyContext(_context);
    }
}