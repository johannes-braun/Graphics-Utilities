#pragma once

#include <array>
#include <jpu/memory>
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#if defined(IO_API_VULKAN)
#include "gui_impl/gui_vk.hpp"
#include "vulkan/logical_device.hpp"
#elif defined(IO_API_OPENGL)
#include "gui_impl/gui_gl.hpp"
#else
#include "gui_impl/gui_none.hpp"
#endif
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

#if defined(IO_API_VULKAN)
        gui(GLFWwindow* window, vkn::LogicalDevice* device, vkn::Swapchain* swapchain) : _window(window), _render_interface(device, swapchain)
        {
            init();
        }
#elif defined(IO_API_OPENGL)
        gui(GLFWwindow* window)
            : _window(window)
        {
            init();
        }
#endif
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

        auto&& render_interface() { return _render_interface; }
        const auto& render_interface() const { return _render_interface; }

    private:

        void init_atlas(){
            ImGui::GetIO().Fonts->TexID = _render_interface.build_font_atlas();
        }
        void init();
        void pre_render(ImDrawData* draw_data){
            _render_interface.pre_render(draw_data);
        }
        void mid_render(const ImDrawCmd& pcmd, int idx_buffer_offset, int vtx_buffer_offset){
            _render_interface.render(pcmd, idx_buffer_offset, vtx_buffer_offset);
        }
        void post_render(){
            _render_interface.post_render();
        }
        void render_data(ImDrawData* data);
        void apply_theme() const;

        ImGuiContext* _context;
        GLFWwindow* _window;

        double _last_time = 0;
        bool _has_frame = false;
        mutable std::array<bool, 3> _mouse_button_states;
        mutable float _mouse_wheel_delta = 0;

#if defined(IO_API_VULKAN)
        impl::gui_vk _render_interface;
#elif defined(IO_API_OPENGL)
        impl::gui_gl _render_interface;
#else
        impl::gui_none _render_interface;
#endif
    };
}

#include "gui.inl"