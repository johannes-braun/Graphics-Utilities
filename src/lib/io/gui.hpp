#pragma once

#include <array>
#include "imgui.h"
#include "imgui_internal.h"
#include "opengl/vertex_array.hpp"
#include "opengl/buffer.hpp"
#include "opengl/texture.hpp"
#include "opengl/pipeline.hpp"

struct GLFWwindow;
struct ImGuiContext;
struct ImDrawData;

namespace io
{
	// A wrapper which enables the rendering of ImGui elements via OpenGL.
	// INFO: A Gui Instance is found in every ansim::Window.
	class gui : public jpu::ref_count
	{
	public:
        constexpr static int font_default = 0;
        constexpr static int font_large_light = 1;
        constexpr static int font_title = 2;

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
		void render_data(ImDrawData* data);
		void apply_theme() const;
		
		ImGuiContext* _context;
        GLFWwindow* _window;

        jpu::ref_ptr<gl::graphics_pipeline> _graphics_pipeline;
        jpu::ref_ptr<gl::vertex_array> _vertex_array;
        jpu::ref_ptr<gl::buffer> _vertex_buffer;
        jpu::ref_ptr<gl::buffer> _index_buffer;
        jpu::ref_ptr<gl::texture> _fonts_atlas;
        jpu::ref_ptr<gl::sampler> _sampler;

        size_t _last_vertex_buffer_size = 0;
        size_t _last_index_buffer_size = 0;
        double _last_time = 0;
		bool _has_frame = false;
		mutable std::array<bool, 3> _mouse_button_states;
		mutable float _mouse_wheel_delta = 0;
	};
}