#include "gui.hpp"

#include <vulkan/device.hpp>

namespace gfx
{
    constexpr std::array<wf::keycode, 3> mouse_buttons{
        wf::mouse_left,
        wf::mouse_right,
        wf::mouse_middle
    };

    void gui::init()
    {
        // Save the default ImGui context.
        // At execution termination, restore the default context and Shutdown ImGui.
        // This is needed because for some reason ImGui needs a valid context on ImGui::Shutdown().
        static class gui_init
        {
        public:
            gui_init() : _m_default_context(ImGui::GetCurrentContext()) {}
            ~gui_init()
            {
                ImGui::SetCurrentContext(_m_default_context);
                ImGui::Shutdown();
            }
        private:
            ImGuiContext * _m_default_context;
        } imgui_init;

        _context = ImGui::CreateContext();
        ImGui::SetCurrentContext(_context);

        auto&& io = _context->IO;
        io.KeyMap[ImGuiKey_Tab] = wf::key_tab;                         // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
        io.KeyMap[ImGuiKey_LeftArrow] = wf::key_left;
        io.KeyMap[ImGuiKey_RightArrow] = wf::key_right;
        io.KeyMap[ImGuiKey_UpArrow] = wf::key_up;
        io.KeyMap[ImGuiKey_DownArrow] = wf::key_down;
        io.KeyMap[ImGuiKey_PageUp] = wf::key_prior;
        io.KeyMap[ImGuiKey_PageDown] = wf::key_next;
        io.KeyMap[ImGuiKey_Home] = wf::key_home;
        io.KeyMap[ImGuiKey_End] = wf::key_end;
        io.KeyMap[ImGuiKey_Delete] = wf::key_delete;
        io.KeyMap[ImGuiKey_Backspace] = wf::key_back;
        io.KeyMap[ImGuiKey_Enter] = wf::key_return;
        io.KeyMap[ImGuiKey_Escape] = wf::key_escape;
        io.KeyMap[ImGuiKey_A] = 'A';
        io.KeyMap[ImGuiKey_C] = 'C';
        io.KeyMap[ImGuiKey_V] = 'V';
        io.KeyMap[ImGuiKey_X] = 'X';
        io.KeyMap[ImGuiKey_Y] = 'Y';
        io.KeyMap[ImGuiKey_Z] = 'Z';
        apply_theme();

        io.UserData = this;
        io.RenderDrawListsFn = nullptr;

        io.SetClipboardTextFn = [](void* data, const char* text) {
            wf::set_clipboard_text(std::wstring(text, text + strlen(text))); 
        };
        io.GetClipboardTextFn = [](void* data) {
            static std::string text;
            auto cb = wf::get_clipboard_text();
            text = { cb.begin(), cb.end() };
            return text.c_str();
        };

        init_atlas();

        _window->key_callbacks.emplace_back([this](int key, bool pressed, bool alt) {
            auto&& io = _context->IO;

            if (auto it = std::find(mouse_buttons.begin(), mouse_buttons.end(), key); it != mouse_buttons.end() && pressed)
            {
                _mouse_button_states[it - mouse_buttons.begin()] = true;
                return;
            }

            io.KeysDown[key] = pressed;
            io.KeyCtrl = io.KeysDown[wf::key_lcontrol] || io.KeysDown[wf::key_rcontrol] || io.KeysDown[wf::key_control];
            io.KeyShift = io.KeysDown[wf::key_lshift] || io.KeysDown[wf::key_rshift] || io.KeysDown[wf::key_shift];
            io.KeyAlt = alt;
            io.KeySuper = io.KeysDown[wf::key_lwin] || io.KeysDown[wf::key_rwin];
        });
        _window->scroll_callbacks.emplace_back([this](float sx, float sy) {
            _mouse_wheel_delta += sy;
        });
        _window->char_callbacks.emplace_back([this](wchar_t c) {
            auto&& io = _context->IO;
            if (c > 0 && c < 0x10000)
                io.AddInputCharacter(c);
        });
    }

    void gui::new_frame()
    {
        _has_frame = true;
        ImGui::SetCurrentContext(_context);

        auto&& io = _context->IO;
        const wf::rect r = _window->get_rect();
        io.DisplaySize = ImVec2(float(r.right-r.left), float(r.bottom - r.top));
        io.DisplayFramebufferScale = ImVec2(1, 1);

        io.DeltaTime = float(_window->time_update().count());

        if (_window->has_focus())
        {
            if (io.WantMoveMouse)
            {
                _window->set_cursor_position(int(io.MousePos.x), int(io.MousePos.y));
            }
            else
            {
                const wf::point c = _window->cursor_position();
                io.MousePos = ImVec2(float(c.x), float(c.y));
            }
        }
        else
        {
            io.MousePos = ImVec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
        }

        for (int i = 0; i < 3; i++)
        {
            // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
            io.MouseDown[i] = _mouse_button_states[i] ||  wf::is_key_down(mouse_buttons[i]);
            _mouse_button_states[i] = false;
        }

        io.MouseWheel = _mouse_wheel_delta;
        _mouse_wheel_delta = 0;

        ImGui::NewFrame();
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

    gui::gui(std::shared_ptr<wf::window> window, vkn::device* device, vkn::swapchain* swapchain) : _window(window),
        _vk_impl(io::impl::gui_vk(device, swapchain)),
        _api(io::api::vulkan)
    {
        init();
    }

    gui::gui(std::shared_ptr<wf::window> window) : _window(window), _gl_impl(io::impl::gui_gl(true)), _api(io::api::opengl)
    {
        init();
    }

    io::impl::gui_gl& gui::render_interface_gl()
    {
        return _gl_impl;
    }

    const io::impl::gui_gl& gui::render_interface_gl() const
    {
        return _gl_impl;
    }

    io::impl::gui_vk& gui::render_interface_vk()
    {
        return _vk_impl;
    }

    const io::impl::gui_vk& gui::render_interface_vk() const
    {
        return _vk_impl;
    }

    void gui::init_atlas()
    {
        switch (_api)
        {
        case io::api::opengl: _gl_impl.build_font_atlas();
            ImGui::GetIO().Fonts->TexID = _gl_impl.build_font_atlas();
            break;
        case io::api::vulkan:
            ImGui::GetIO().Fonts->TexID = _vk_impl.build_font_atlas();
            break;
        }
    }

    void gui::pre_render(ImDrawData* draw_data)
    {
        switch (_api)
        {
        case io::api::opengl:
            _gl_impl.pre_render(draw_data);
            break;
        case io::api::vulkan:
            _vk_impl.pre_render(draw_data);
            break;
        }
    }

    void gui::mid_render(const ImDrawCmd& pcmd, const int idx_buffer_offset, const int vtx_buffer_offset)
    {
        switch (_api)
        {
        case io::api::opengl:
            _gl_impl.render(pcmd, idx_buffer_offset, vtx_buffer_offset);
            break;
        case io::api::vulkan:
            _vk_impl.render(pcmd, idx_buffer_offset, vtx_buffer_offset);
            break;
        }
    }

    void gui::post_render()
    {
        switch (_api)
        {
        case io::api::opengl:
            _gl_impl.post_render();
            break;
        case io::api::vulkan:
            _vk_impl.post_render();
            break;
        }
    }

    gui::~gui()
    {
        if (_context)
            ImGui::DestroyContext(_context);
    }
}