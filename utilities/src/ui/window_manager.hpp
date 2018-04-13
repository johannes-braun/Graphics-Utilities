#pragma once

#include <vector>
#include <map>
#include <memory>
#include <io/window.hpp>
#include "ui.hpp"
#include "window.hpp"
#include "opengl/pipeline.hpp"

namespace gfx::ui
{
    struct window_info
    {
        window* window = nullptr;
        int layer = 0;
    };

    enum mouse_state
    {
        MOUSE_UP       = 0,
        MOUSE_RELEASE  = 1,
        MOUSE_DOWN     = 2,
        MOUSE_PRESS    = 3,
    };

    class window_manager
    {
    public:
        window_manager(const std::shared_ptr<io::window>& base);
        void bring_to_front(const std::wstring& name);
        window_info& info(const std::wstring& name);
        window* front_window_at(float x, float y);
        const std::wstring* front_window_name_at(float x, float y);

        io::window& get_base_window();
        window* make_window(const std::wstring& name, rect initial_rect);
        window* get_window(const std::wstring& name) const noexcept;
        
        void render();

        glm::vec2 get_screen_size() const noexcept;
        rect get_maximal_rect() const noexcept;

        mouse_state get_mouse_left_state() const noexcept;
        glm::vec2 get_cursor_position() const noexcept;

        glm::vec2 minimize_position() const noexcept;
        void notify_minimize(window* w);
        void notify_unminimize(window* w);

    private:
        mouse_state _mouse_left_state = MOUSE_UP;
        window* _action_window = nullptr;
        std::function<void()> _action;

        glm::vec2 _minimize_position{ 48, 0 };

        draw_list _ui_draw_list;

        std::vector<window*> _minimized;
        std::shared_ptr<io::window> _base_window;
        std::vector<std::unique_ptr<window>> _window_storage;
        std::unordered_map<std::wstring, window_info> _windows;
        gl::pipeline _base_pipeline;
    };
}