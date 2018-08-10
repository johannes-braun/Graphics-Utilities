#include "gui.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <gfx/file/file.hpp>

namespace gfx {
gui::gui() noexcept
{
    _gfx_context = gfx::context::current();
    _context     = ImGui::CreateContext();
    ImGui::SetCurrentContext(_context);

    auto&& io                      = _context->IO;
    io.KeyMap[ImGuiKey_Tab]        = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow]  = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow]    = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow]  = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp]     = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown]   = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home]       = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End]        = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Delete]     = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace]  = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter]      = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape]     = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A]          = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C]          = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V]          = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X]          = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y]          = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z]          = GLFW_KEY_Z;
    apply_theme();

    io.UserData           = this;
    io.RenderDrawListsFn  = nullptr;
    io.ClipboardUserData  = this;
    io.SetClipboardTextFn = [](void* data, const char* text) {
        glfwSetClipboardString(reinterpret_cast<gui*>(data)->_gfx_context->window(), text);
    };
    io.GetClipboardTextFn = [](void* data) { return glfwGetClipboardString(reinterpret_cast<gui*>(data)->_gfx_context->window()); };

    _key_callback = _gfx_context->key_callback.add([this](GLFWwindow*, int key, int, int action, int mods) {
        auto&& io = _context->IO;
        if (action == GLFW_PRESS) io.KeysDown[key] = true;
        if (action == GLFW_RELEASE) io.KeysDown[key] = false;

        io.KeyCtrl  = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
        io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
        io.KeyAlt   = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
        io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
    });

    _mousebutton_callback = _gfx_context->mouse_button_callback.add([this](GLFWwindow*, int button, int action, int mods) {
        auto&& io = _context->IO;
        if (action == GLFW_PRESS && button >= 0 && button < 3) _mouse_button_states[button] = true;
        return io.WantCaptureMouse;
    });

    _scroll_callback = _gfx_context->scroll_callback.add([this](GLFWwindow*, double x, double y) { _mouse_wheel_delta += float(y); });

    _char_callback = _gfx_context->char_callback.add([this](GLFWwindow*, uint32_t c) {
        auto&& io = _context->IO;
        if (c > 0 && c < 0x10000) io.AddInputCharacter(static_cast<unsigned short>(c));
    });

    unsigned char* pixels;
    int            width, height;
    ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    gfx::host_image stage(gfx::rgba8unorm, extent(width, height));
    memcpy(stage.storage().data(), pixels, width * height * 4);

    _font_atlas = std::make_unique<device_image>(stage, 1);
    _sampler.set_filter(filter_mode::min, filter::linear);
    _sampler.set_filter(filter_mode::mag, filter::linear);
    _sampler.set_lod(lod::max, 1.f);

    _font_atlas_view            = std::make_unique<image_view>(imgv_type::image2d, *_font_atlas);
    ImGui::GetIO().Fonts->TexID = _font_atlas_view.get();

    _pipe_vertex_bindings.attributes.emplace_back(0, 0, rg32f, offsetof(ImDrawVert, pos));
    _pipe_vertex_bindings.attributes.emplace_back(1, 0, rg32f, offsetof(ImDrawVert, uv));
    _pipe_vertex_bindings.attributes.emplace_back(2, 0, rgba8unorm, offsetof(ImDrawVert, col));
    _pipe_vertex_bindings.bindings.emplace_back(0, sizeof(ImDrawVert));
    _pipe_state.state_vertex_input = &_pipe_vertex_bindings;

    _data_layout.push(binding_type::uniform_buffer);
    _push_layout.push(binding_type::sampled_image);
    _render_layout = renderpass_layout();
    _render_layout.add_color_attachment(bgra8unorm);


    for (auto& iv : _gfx_context->swapchain()->image_views()) {
        _per_frame_infos.emplace_back(_data_layout, _gfx_context->options().window_width, _gfx_context->options().window_height,
                                      _render_layout);
        _per_frame_infos.back().fbo.attach(attachment::color, 0, iv, load{});
        _per_frame_infos.back().data_set.bind(0, _per_frame_infos.back().data);
    }

	_gfx_context->framebuffer_size_callback.add([this](GLFWwindow* w, int x, int y) {
		for (int i=0; i<_per_frame_infos.size(); ++i)
		{
			_per_frame_infos[i].fbo = gfx::framebuffer(_gfx_context->options().window_width, _gfx_context->options().window_height, 1,
				_render_layout);
			_per_frame_infos[i].fbo.attach(attachment::color, 0, _gfx_context->swapchain()->image_views()[i], load{});
		}
	});

    auto shaders2 = {shader(shader_type::vert, "imgui/imgui.vert"), shader(shader_type::frag, "imgui/imgui.frag")};

    pipe_state::binding_layouts bnd;
    _pipe_state.state_bindings = &bnd;
    _pipe_state.state_bindings->layouts.push_back(&_data_layout);
    _pipe_state.state_bindings->layouts.push_back(&_push_layout);

    pipe_state::blending          blend;
    pipe_state::blend_attachment& att = blend.attachments.emplace_back();
    att.blendEnable                   = true;
    att.colorBlendOp                  = blend_op::op_add;
    att.srcColorBlendFactor           = blend_factor::src_alpha;
    att.dstColorBlendFactor           = blend_factor::one_minus_src_alpha;
    _pipe_state.state_blending        = &blend;

    _pipeline = std::make_unique<graphics_pipeline>(_pipe_state, _render_layout, shaders2);
}

gui::~gui()
{
    _gfx_context->key_callback.remove(_key_callback);
    _gfx_context->char_callback.remove(_char_callback);
    _gfx_context->mouse_button_callback.remove(_mousebutton_callback);
    _gfx_context->scroll_callback.remove(_scroll_callback);
}

void gui::new_frame()
{
    _gfx_context->make_current();
    ImGui::SetCurrentContext(_context);

    auto&& io = _context->IO;
    int    w, h;
    int    display_w, display_h;
    glfwGetWindowSize(_gfx_context->window(), &w, &h);
    glfwGetFramebufferSize(_gfx_context->window(), &display_w, &display_h);
    io.DisplaySize             = ImVec2(static_cast<float>(w), static_cast<float>(h));
    io.DisplayFramebufferScale = ImVec2(w > 0 ? (static_cast<float>(display_w) / w) : 0, h > 0 ? (static_cast<float>(display_h) / h) : 0);
    io.DeltaTime               = static_cast<float>(glfwGetTime() - _last_time);
    _last_time                 = static_cast<float>(glfwGetTime());

    if (glfwGetWindowAttrib(_gfx_context->window(), GLFW_FOCUSED)) {
        if (io.WantSetMousePos) {
            glfwSetCursorPos(_gfx_context->window(), static_cast<double>(io.MousePos.x), static_cast<double>(io.MousePos.y));
        }
        else
        {
            double mouse_x, mouse_y;
            glfwGetCursorPos(_gfx_context->window(), &mouse_x, &mouse_y);
            io.MousePos = ImVec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
        }
    }
    else
    {
        io.MousePos = ImVec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
    }

    for (int i = 0; i < 3; i++) {
        // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter
        // than 1 frame.
        io.MouseDown[i]         = _mouse_button_states[i] || glfwGetMouseButton(_gfx_context->window(), i) != 0;
        _mouse_button_states[i] = false;
    }

    io.MouseWheel      = _mouse_wheel_delta;
    _mouse_wheel_delta = 0;

    ImGui::NewFrame();
}

void gui::render() const
{
	auto& frame = _per_frame_infos[_gfx_context->swapchain()->current_image()];
	frame.render_commands.reset();
	frame.render_commands.begin();
	render(frame.render_commands);
	frame.render_commands.end();
	frame.render_commands.execute();
}

void gui::render(commands& cmd) const
{
    ImGui::SetCurrentContext(_context);
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();

    auto&&    io        = _context->IO;
    const int fb_width  = static_cast<int>(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    const int fb_height = static_cast<int>(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0) return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    const auto vertex_count = draw_data->TotalVtxCount;
    const auto index_count  = draw_data->TotalIdxCount;
    if (vertex_count * index_count == 0) return;

    auto& frame = _per_frame_infos[_gfx_context->swapchain()->current_image()];
    frame.vbo.clear();
    frame.ibo.clear();

    for (int list = 0; list < draw_data->CmdListsCount; ++list) {
        const auto cmd_list = draw_data->CmdLists[list];

        for (auto& d : cmd_list->VtxBuffer) frame.vbo.push_back(d);
        for (auto& i : cmd_list->IdxBuffer) frame.ibo.push_back(i);
    }

    frame.data[0]       = glm::mat4(1.f);
    frame.data[0][0][0] = 2.0f / io.DisplaySize.x;
    frame.data[0][1][1] = 2.0f / io.DisplaySize.y;
    frame.data[0][2]    = glm::vec4{0.0f, 0.0f, -1.0f, 0.0f};
    frame.data[0][3]    = glm::vec4{-1.0f, -1.0f, 0.0f, 1.0f};

    cmd.begin_pass(frame.fbo);

    viewport vp[]{{0, 0, io.DisplaySize.x, io.DisplaySize.y, 0.f, 1.f}};

	cmd.bind_pipeline(*_pipeline, {&frame.data_set});
	cmd.bind_vertex_buffer(frame.vbo, 0);
	cmd.bind_index_buffer(frame.ibo, index_type::uint16);
    size_t vtx_buffer_offset = 0;
    size_t idx_buffer_offset = 0;
    for (auto n = 0; n < draw_data->CmdListsCount; n++) {
        const auto cmd_list = draw_data->CmdLists[n];

        for (auto cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
            const auto& pcmd = cmd_list->CmdBuffer[cmd_i];
            if (pcmd.UserCallback) {
                pcmd.UserCallback(cmd_list, &pcmd);
            }
            else
            {
                const image_view* iv = static_cast<const image_view*>(pcmd.TextureId);
                cmd.push_binding(1, 0, binding_type::sampled_image, *iv, _sampler);
                rect2f sc[]{{glm::vec2(pcmd.ClipRect.x, pcmd.ClipRect.y), glm::vec2(pcmd.ClipRect.z, pcmd.ClipRect.w)}};
                cmd.set_viewports(0, vp, sc);
                cmd.draw_indexed(pcmd.ElemCount, 1, u32(idx_buffer_offset), u32(vtx_buffer_offset));
            }
            idx_buffer_offset += pcmd.ElemCount;
        }
        vtx_buffer_offset += cmd_list->VtxBuffer.Size;
    }
    cmd.end_pass();
}

void gui::apply_theme()
{
    auto atlas = _context->IO.Fonts;

    // Default Font
    file font_file("ui/fonts/Poppins-SemiBold.ttf");
    _font_default = atlas->AddFontFromFileTTF(font_file.path.string().c_str(), 14);
    _font_small = atlas->AddFontFromFileTTF(font_file.path.string().c_str(), 13);
    _font_large = atlas->AddFontFromFileTTF(font_file.path.string().c_str(), 16);
    _font_huge = atlas->AddFontFromFileTTF(font_file.path.string().c_str(), 24);

    atlas->Build();

    _context->IO.FontDefault = atlas->Fonts[0];

    auto&& style             = &_context->Style;
    style->WindowPadding     = ImVec2(8, 8);
    style->WindowRounding    = 1.f;
    style->FrameRounding     = 2.0f;
    style->FramePadding      = ImVec2(8, 4);
    style->ItemSpacing       = ImVec2(6, 4);
    style->ItemInnerSpacing  = ImVec2(4, 3);
    style->IndentSpacing     = 25.0f;
    style->ScrollbarSize     = 12.0f;
    style->ScrollbarRounding = 9.0f;
    style->GrabMinSize       = 5.0f;
    style->GrabRounding      = 2.0f;

    style->Colors[ImGuiCol_Text]                 = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled]         = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_WindowBg]             = ImVec4(0.06f, 0.05f, 0.07f, 0.50f);
    style->Colors[ImGuiCol_ChildWindowBg]        = ImVec4(0.07f, 0.07f, 0.09f, 0.50f);
    style->Colors[ImGuiCol_PopupBg]              = ImVec4(0.07f, 0.07f, 0.09f, 0.50f);
    style->Colors[ImGuiCol_Border]               = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);    // 0.80f, 0.80f, 0.83f, 0.88f
    style->Colors[ImGuiCol_BorderShadow]         = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    style->Colors[ImGuiCol_FrameBg]              = ImVec4(5.f * 0.05f, 5.f * 0.09f, 5.f * 0.12f, 1.00f);
    style->Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_TitleBg]              = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    style->Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.07f, 0.07f, 0.09f, 0.50f);
    style->Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.06f, 0.05f, 0.07f, 0.50f);
    style->Colors[ImGuiCol_CheckMark]            = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrab]           = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.06f, 0.05f, 0.07f, 0.50f);
    style->Colors[ImGuiCol_Button]               = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_ButtonActive]         = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_Header]               = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_HeaderActive]         = ImVec4(0.06f, 0.05f, 0.07f, 0.50f);
    style->Colors[ImGuiCol_Column]               = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ColumnHovered]        = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_ColumnActive]         = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style->Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.06f, 0.05f, 0.07f, 0.50f);
    style->Colors[ImGuiCol_PlotLines]            = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
    style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}
}    // namespace gfx
