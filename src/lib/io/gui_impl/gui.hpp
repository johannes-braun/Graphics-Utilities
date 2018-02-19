#pragma once

#include "io/api.hpp"
#include "gui_vk.hpp"
#include "gui_gl.hpp"

#include <array>
#include <jpu/memory>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <GLFW/glfw3.h>

namespace io
{
    // A wrapper which enables the rendering of ImGui elements via OpenGL.
    // INFO: A Gui Instance is found in every Window.
    class gui : public jpu::ref_count
    {
    public:
        constexpr static int font_default = 0;
        constexpr static int font_large_light = 1;
        constexpr static int font_title = 2;

        gui(GLFWwindow* window, vkn::device* device, vkn::swapchain* swapchain);

        gui(GLFWwindow* window);

        ~gui();

        // Is called each frame and resets the current input states.
        void new_frame();

        // Returns true if key input should be blocked by the GUI.
        bool key_action(int key, int action, int mods) const;

        // Returns true if mouse button input should be blocked by the GUI.
        bool mouse_button_action(int button, int action, int mods) const;

        void scrolled(double delta_y) const;

        // Returns true if character input should be blocked by the GUI.
        bool char_input(wchar_t character) const;

        // Returns true, if newFrame(...) is called at least once.
        bool is_initialized() const;

        void render();

    private:

        void init_atlas();
        void init();
        void pre_render(ImDrawData* draw_data);

        void mid_render(const ImDrawCmd& pcmd, const int idx_buffer_offset, const int vtx_buffer_offset);

        void post_render();
        void render_data(ImDrawData* data);
        void apply_theme() const;

        ImGuiContext* _context;
        GLFWwindow* _window;

        double _last_time = 0;
        bool _has_frame = false;
        mutable std::array<bool, 3> _mouse_button_states;
        mutable float _mouse_wheel_delta = 0;

        impl::gui_gl _gl_impl{ false };
        impl::gui_vk _vk_impl;
        api _api;

    public:
        impl::gui_gl& render_interface_gl();
        const impl::gui_gl& render_interface_gl() const;

        impl::gui_vk& render_interface_vk();
        const impl::gui_vk& render_interface_vk() const;
    };
}