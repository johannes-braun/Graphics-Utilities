//#include "../../imgui.hpp"
//#include "../../log.hpp"
//#include <gfx/imgui/imgui.h>
//#include <gfx/imgui/imgui_internal.h>
//#include <gfx/file.hpp>
//
//namespace gfx
//{
//imgui::imgui() noexcept
//{
//    _gfx_context = gfx::context::current();
//    _context     = ImGui::CreateContext();
//    ImGui::SetCurrentContext(_context);
//
//    auto&& io                      = _context->IO;
//    io.KeyMap[ImGuiKey_Tab]        = GLFW_KEY_TAB;
//    io.KeyMap[ImGuiKey_LeftArrow]  = GLFW_KEY_LEFT;
//    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
//    io.KeyMap[ImGuiKey_UpArrow]    = GLFW_KEY_UP;
//    io.KeyMap[ImGuiKey_DownArrow]  = GLFW_KEY_DOWN;
//    io.KeyMap[ImGuiKey_PageUp]     = GLFW_KEY_PAGE_UP;
//    io.KeyMap[ImGuiKey_PageDown]   = GLFW_KEY_PAGE_DOWN;
//    io.KeyMap[ImGuiKey_Home]       = GLFW_KEY_HOME;
//    io.KeyMap[ImGuiKey_End]        = GLFW_KEY_END;
//    io.KeyMap[ImGuiKey_Delete]     = GLFW_KEY_DELETE;
//    io.KeyMap[ImGuiKey_Backspace]  = GLFW_KEY_BACKSPACE;
//    io.KeyMap[ImGuiKey_Enter]      = GLFW_KEY_ENTER;
//    io.KeyMap[ImGuiKey_Escape]     = GLFW_KEY_ESCAPE;
//    io.KeyMap[ImGuiKey_A]          = GLFW_KEY_A;
//    io.KeyMap[ImGuiKey_C]          = GLFW_KEY_C;
//    io.KeyMap[ImGuiKey_V]          = GLFW_KEY_V;
//    io.KeyMap[ImGuiKey_X]          = GLFW_KEY_X;
//    io.KeyMap[ImGuiKey_Y]          = GLFW_KEY_Y;
//    io.KeyMap[ImGuiKey_Z]          = GLFW_KEY_Z;
//    apply_theme();
//
//    io.UserData           = this;
//    io.RenderDrawListsFn  = nullptr;
//    io.ClipboardUserData  = this;
//    io.SetClipboardTextFn = [](void* data, const char* text) {
//        glfwSetClipboardString(reinterpret_cast<imgui*>(data)->_gfx_context->window(), text);
//    };
//    io.GetClipboardTextFn = [](void* data) { return glfwGetClipboardString(reinterpret_cast<imgui*>(data)->_gfx_context->window()); };
//
//    _key_callback = _gfx_context->key_callback.add([this](GLFWwindow*, int key, int, int action, int mods) {
//        auto&& io = _context->IO;
//        if(action == GLFW_PRESS)
//            io.KeysDown[key] = true;
//        if(action == GLFW_RELEASE)
//            io.KeysDown[key] = false;
//
//        io.KeyCtrl  = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
//        io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
//        io.KeyAlt   = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
//        io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
//    });
//
//    _mousebutton_callback = _gfx_context->mouse_button_callback.add([this](GLFWwindow*, int button, int action, int mods) {
//        auto&& io = _context->IO;
//        if(action == GLFW_PRESS && button >= 0 && button < 3)
//            _mouse_button_states[button] = true;
//        return io.WantCaptureMouse;
//    });
//
//    _scroll_callback = _gfx_context->scroll_callback.add([this](GLFWwindow*, double x, double y) { _mouse_wheel_delta += float(y); });
//
//    _char_callback = _gfx_context->char_callback.add([this](GLFWwindow*, uint32_t c) {
//        auto&& io = _context->IO;
//        if(c > 0 && c < 0x10000)
//            io.AddInputCharacter(static_cast<unsigned short>(c));
//    });
//
//    if(_gfx_context->options().graphics_api == gapi::opengl)
//        _handler = std::make_unique<imgui_handler_opengl>(*this);
//    else if(_gfx_context->options().graphics_api == gapi::vulkan)
//        _handler = std::make_unique<imgui_handler_vulkan>(*this);
//}
//
//imgui::~imgui()
//{
//    _gfx_context->key_callback.remove(_key_callback);
//    _gfx_context->char_callback.remove(_char_callback);
//    _gfx_context->mouse_button_callback.remove(_mousebutton_callback);
//    _gfx_context->scroll_callback.remove(_scroll_callback);
//}
//
//void imgui::new_frame()
//{
//    _gfx_context->make_current();
//    ImGui::SetCurrentContext(_context);
//
//    auto&& io = _context->IO;
//    int    w, h;
//    int    display_w, display_h;
//    glfwGetWindowSize(_gfx_context->window(), &w, &h);
//    glfwGetFramebufferSize(_gfx_context->window(), &display_w, &display_h);
//    io.DisplaySize             = ImVec2(static_cast<float>(w), static_cast<float>(h));
//    io.DisplayFramebufferScale = ImVec2(w > 0 ? (static_cast<float>(display_w) / w) : 0, h > 0 ? (static_cast<float>(display_h) / h) : 0);
//    io.DeltaTime               = static_cast<float>(glfwGetTime() - _last_time);
//    _last_time                 = static_cast<float>(glfwGetTime());
//
//    if(glfwGetWindowAttrib(_gfx_context->window(), GLFW_FOCUSED))
//    {
//        if(io.WantSetMousePos)
//        {
//            glfwSetCursorPos(_gfx_context->window(), static_cast<double>(io.MousePos.x), static_cast<double>(io.MousePos.y));
//        }
//        else
//        {
//            double mouse_x, mouse_y;
//            glfwGetCursorPos(_gfx_context->window(), &mouse_x, &mouse_y);
//            io.MousePos = ImVec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
//        }
//    }
//    else
//    {
//        io.MousePos = ImVec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
//    }
//
//    for(int i = 0; i < 3; i++)
//    {
//        // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter
//        // than 1 frame.
//        io.MouseDown[i]         = _mouse_button_states[i] || glfwGetMouseButton(_gfx_context->window(), i) != 0;
//        _mouse_button_states[i] = false;
//    }
//
//    io.MouseWheel      = _mouse_wheel_delta;
//    _mouse_wheel_delta = 0;
//
//    ImGui::NewFrame();
//}
//
//void imgui::render() const
//{
//    ImGui::SetCurrentContext(_context);
//    ImGui::Render();
//    ImDrawData* draw_data = ImGui::GetDrawData();
//
//    auto&&    io        = _context->IO;
//    const int fb_width  = static_cast<int>(io.DisplaySize.x * io.DisplayFramebufferScale.x);
//    const int fb_height = static_cast<int>(io.DisplaySize.y * io.DisplayFramebufferScale.y);
//    if(fb_width == 0 || fb_height == 0)
//        return;
//    draw_data->ScaleClipRects(io.DisplayFramebufferScale);
//
//    const auto vertex_count = draw_data->TotalVtxCount;
//    const auto index_count  = draw_data->TotalIdxCount;
//    if(vertex_count * index_count == 0)
//        return;
//
//    _handler->upload(draw_data);
//
//    size_t vtx_buffer_offset = 0;
//    size_t idx_buffer_offset = 0;
//    for(auto n = 0; n < draw_data->CmdListsCount; n++)
//    {
//        const auto cmd_list = draw_data->CmdLists[n];
//
//        for(auto cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
//        {
//            const auto& pcmd = cmd_list->CmdBuffer[cmd_i];
//            if(pcmd.UserCallback)
//            {
//                pcmd.UserCallback(cmd_list, &pcmd);
//            }
//            else
//            {
//                _handler->draw(pcmd, uint32_t(idx_buffer_offset), uint32_t(vtx_buffer_offset));
//            }
//            idx_buffer_offset += pcmd.ElemCount;
//        }
//        vtx_buffer_offset += cmd_list->VtxBuffer.Size;
//    }
//
//    _handler->finalize();
//}
//
//void imgui::apply_theme() const
//{
//    auto atlas = _context->IO.Fonts;
//
//    // Default Font
//    file font_file("ui/fonts/Ruda-Bold.ttf");
//    atlas->AddFontFromFileTTF(font_file.path.string().c_str(), 12);
//    atlas->AddFontFromFileTTF(font_file.path.string().c_str(), 10);
//    atlas->AddFontFromFileTTF(font_file.path.string().c_str(), 14);
//    atlas->AddFontFromFileTTF(font_file.path.string().c_str(), 18);
//
//    atlas->Build();
//
//    _context->IO.FontDefault = atlas->Fonts[0];
//
//    auto&& style             = &_context->Style;
//	style->WindowPadding = ImVec2(8, 8);
//	style->WindowRounding = 1.f;
//	style->FrameRounding     = 2.0f;
//	style->FramePadding = ImVec2(8, 4);
//	style->ItemSpacing = ImVec2(6, 4);
//	style->ItemInnerSpacing = ImVec2(4, 3);
//	style->IndentSpacing     = 25.0f;
//	style->ScrollbarSize     = 12.0f;
//	style->ScrollbarRounding = 9.0f;
//	style->GrabMinSize       = 5.0f;
//	style->GrabRounding      = 2.0f;
//
//    style->Colors[ImGuiCol_Text]                 = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
//    style->Colors[ImGuiCol_TextDisabled]         = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
//    style->Colors[ImGuiCol_WindowBg]             = ImVec4(0.06f, 0.05f, 0.07f, 0.50f);
//    style->Colors[ImGuiCol_ChildWindowBg]        = ImVec4(0.07f, 0.07f, 0.09f, 0.50f);
//    style->Colors[ImGuiCol_PopupBg]              = ImVec4(0.07f, 0.07f, 0.09f, 0.50f);
//    style->Colors[ImGuiCol_Border]               = ImVec4(0.80f, 0.80f, 0.83f, 0.88f); // 0.80f, 0.80f, 0.83f, 0.88f
//    style->Colors[ImGuiCol_BorderShadow]         = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
//    style->Colors[ImGuiCol_FrameBg]              = ImVec4(5.f*0.05f, 5.f*0.09f, 5.f*0.12f, 1.00f);
//    style->Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
//    style->Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
//    style->Colors[ImGuiCol_TitleBg]              = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
//    style->Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
//    style->Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.07f, 0.07f, 0.09f, 0.50f);
//    style->Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
//    style->Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
//    style->Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
//    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
//    style->Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.06f, 0.05f, 0.07f, 0.50f);
//    style->Colors[ImGuiCol_CheckMark]            = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
//    style->Colors[ImGuiCol_SliderGrab]           = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
//    style->Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.06f, 0.05f, 0.07f, 0.50f);
//    style->Colors[ImGuiCol_Button]               = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
//    style->Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
//    style->Colors[ImGuiCol_ButtonActive]         = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
//    style->Colors[ImGuiCol_Header]               = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
//    style->Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
//    style->Colors[ImGuiCol_HeaderActive]         = ImVec4(0.06f, 0.05f, 0.07f, 0.50f);
//    style->Colors[ImGuiCol_Column]               = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
//    style->Colors[ImGuiCol_ColumnHovered]        = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
//    style->Colors[ImGuiCol_ColumnActive]         = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
//    style->Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
//    style->Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
//    style->Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.06f, 0.05f, 0.07f, 0.50f);
//    style->Colors[ImGuiCol_PlotLines]            = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
//    style->Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
//    style->Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
//    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
//    style->Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
//    style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
//}
//}
