#pragma once

#include "context.hpp"
#include "graphics/binding_set.hpp"
#include "graphics/command_list.hpp"
#include "graphics/framebuffer.hpp"
#include "graphics/host_buffer.hpp"
#include "interface/imgui_handler.hpp"

#include "imgui/imgui.h"
#include <GLFW/glfw3.h>
#include <array>
#include <memory>

namespace gfx {
class imgui
{
public:
    imgui() noexcept;
    ~imgui();

    void new_frame();
    void render() const;

private:
    void apply_theme() const;

    std::shared_ptr<gfx::context>  _gfx_context;
    std::unique_ptr<imgui_handler> _handler;
    ImGuiContext*                  _context;
    float                          _last_time = 0;

    std::array<bool, 3> _mouse_button_states;
    float               _mouse_wheel_delta = 0;

    callback<GLFWkeyfun>::glfw_fun_ptr*         _key_callback;
    callback<GLFWcharfun>::glfw_fun_ptr*        _char_callback;
    callback<GLFWmousebuttonfun>::glfw_fun_ptr* _mousebutton_callback;
    callback<GLFWscrollfun>::glfw_fun_ptr*      _scroll_callback;

    std::unique_ptr<device_image> _font_atlas;
    std::unique_ptr<image_view>   _font_atlas_view;
    sampler                       _sampler;

    pipeline_state                     _pipe_state;
    pipeline_state::vertex_input       _pipe_vertex_bindings;
    std::unique_ptr<graphics_pipeline> _pipeline;
    std::unique_ptr<graphics_pipeline> _pipeline_tex;

    binding_layout _data_layout;
    binding_layout _push_layout {true};

    struct frame_infos
    {
        frame_infos(const binding_layout& l, u32 w, u32 h, const renderpass_layout& r) : data_set(l), fbo(w, h, 1, r) {}

        host_buffer<ImDrawVert> vbo;
        host_buffer<ImDrawIdx>  ibo;
        host_buffer<glm::mat4>  data {1};
        binding_set             data_set;

        framebuffer fbo;
        commands    render_commands {commands_type::graphics};
    };

    renderpass_layout                _render_layout;
    mutable std::vector<frame_infos> _per_frame_infos;
};
}    // namespace gfx
