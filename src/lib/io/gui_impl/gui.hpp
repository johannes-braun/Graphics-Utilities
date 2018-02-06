#pragma once

#include <array>
#include <jpu/memory>
#include <any>
#include <variant>
#include "api.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "gui_impl/gui_vk.hpp"
#include "vulkan/device.hpp"
#include "gui_impl/gui_gl.hpp"
#include "GLFW/glfw3.h"

struct GLFWwindow;
struct ImGuiContext;
struct ImDrawData;

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

        gui(GLFWwindow* window, vkn::device* device, vkn::swapchain* swapchain) : _window(window), _vk_impl(impl::gui_vk(device, swapchain)),
            _api(api::vulkan)
        {
            init();
        }

        gui(GLFWwindow* window)
            : _window(window), _gl_impl(impl::gui_gl(true)), _api(api::opengl)
        {
            init();
        }
      
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

        auto&& render_interface_gl() { return _gl_impl; }
        const auto& render_interface_gl() const { return _gl_impl; }

        auto&& render_interface_vk() { return _vk_impl; }
        const auto& render_interface_vk() const { return _vk_impl; }

    private:

        void init_atlas(){
            switch (_api)
            {
            case api::opengl:_gl_impl.build_font_atlas();
                ImGui::GetIO().Fonts->TexID = _gl_impl.build_font_atlas();
                break;
            case api::vulkan:
                ImGui::GetIO().Fonts->TexID = _vk_impl.build_font_atlas();
                break;
            }
        }
        void init();
        void pre_render(ImDrawData* draw_data){
            switch (_api)
            {
            case api::opengl:
                _gl_impl.pre_render(draw_data);
                break;
            case api::vulkan:
                _vk_impl.pre_render(draw_data);
                break;
            }
        }
        void mid_render(const ImDrawCmd& pcmd, int idx_buffer_offset, int vtx_buffer_offset){
            switch (_api)
            {
            case api::opengl:
                _gl_impl.render(pcmd, idx_buffer_offset, vtx_buffer_offset);
                break;
            case api::vulkan:
                _vk_impl.render(pcmd, idx_buffer_offset, vtx_buffer_offset);
                break;
            }
        }
        void post_render(){
            switch (_api)
            {
            case api::opengl:
                _gl_impl.post_render();
                break;
            case api::vulkan:
                _vk_impl.post_render();
                break;
            }
        }
        void render_data(ImDrawData* data);
        void apply_theme() const;

        ImGuiContext* _context;
        GLFWwindow* _window;

        double _last_time = 0;
        bool _has_frame = false;
        mutable std::array<bool, 3> _mouse_button_states;
        mutable float _mouse_wheel_delta = 0;

        //std::any _impl;
        impl::gui_gl _gl_impl{ false };
        impl::gui_vk _vk_impl;
        api _api;
    };
}

#include "gui.inl"