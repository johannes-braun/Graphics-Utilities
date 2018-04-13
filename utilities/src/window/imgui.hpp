#pragma once

#include <io/api.hpp>
#include <io/impl/gui_vk.hpp>
#include <io/impl/gui_gl.hpp>

#include <array>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <window/window.hpp>

namespace gfx
{
    // A wrapper which enables the rendering of ImGui elements.
    class imgui
    {
    public:
        constexpr static int font_default = 0;
        constexpr static int font_large_light = 1;
        constexpr static int font_title = 2;

        imgui(std::shared_ptr<wf::window> window, vkn::device* device, vkn::swapchain* swapchain);
        imgui(std::shared_ptr<wf::window> window);
        ~imgui();

        // Is called each frame and resets the current input states.
        void begin();
        void draw();

    private:
        void init_atlas();
        void init();
        void pre_render(ImDrawData* draw_data);

        void mid_render(const ImDrawCmd& pcmd, const int idx_buffer_offset, const int vtx_buffer_offset);

        void post_render();
        void render_data(ImDrawData* data);
        void apply_theme() const;

        ImGuiContext* _context;
        std::shared_ptr<wf::window> _window;

        double _last_time = 0;
        bool _has_frame = false;
        mutable std::array<bool, 3> _mouse_button_states;
        mutable float _mouse_wheel_delta = 0;

        io::impl::gui_gl _gl_impl{ false };
        io::impl::gui_vk _vk_impl;
        io::api _api;

    public:
        io::impl::gui_gl& render_interface_gl();
        const io::impl::gui_gl& render_interface_gl() const;

        io::impl::gui_vk& render_interface_vk();
        const io::impl::gui_vk& render_interface_vk() const;
    };
}