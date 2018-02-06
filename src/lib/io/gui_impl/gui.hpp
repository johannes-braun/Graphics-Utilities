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

        gui(GLFWwindow* window, vkn::device* device, vkn::swapchain* swapchain) : _window(window), _impl(impl::gui_vk(device, swapchain)),
            _api(api::vulkan)
        {
            init();
        }

        gui(GLFWwindow* window)
            : _window(window), _impl(impl::gui_gl()), _api(api::opengl)
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

        auto&& render_interface() { return _impl; }
        const auto& render_interface() const { return _impl; }

    private:

        void init_atlas(){
            switch (_api)
            {
            case api::opengl:
                ImGui::GetIO().Fonts->TexID = std::get<impl::gui_gl>(_impl).build_font_atlas();
                break;
            case api::vulkan:
                ImGui::GetIO().Fonts->TexID = std::get<impl::gui_vk>(_impl).build_font_atlas();
                break;
            }
        }
        void init();
        void pre_render(ImDrawData* draw_data){
            switch (_api)
            {
            case api::opengl:
                std::get<impl::gui_gl>(_impl).pre_render(draw_data);
                break;
            case api::vulkan:
                std::get<impl::gui_vk>(_impl).pre_render(draw_data);
                break;
            }
        }
        void mid_render(const ImDrawCmd& pcmd, int idx_buffer_offset, int vtx_buffer_offset){
            switch (_api)
            {
            case api::opengl:
                std::get<impl::gui_gl>(_impl).render(pcmd, idx_buffer_offset, vtx_buffer_offset);
                break;
            case api::vulkan:
                std::get<impl::gui_vk>(_impl).render(pcmd, idx_buffer_offset, vtx_buffer_offset);
                break;
            }
        }
        void post_render(){
            switch (_api)
            {
            case api::opengl:
                std::get<impl::gui_gl>(_impl).post_render();
                break;
            case api::vulkan:
                std::get<impl::gui_vk>(_impl).post_render();
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
        std::variant<impl::gui_vk, impl::gui_gl> _impl;
        api _api;
    };
}

#include "gui.inl"