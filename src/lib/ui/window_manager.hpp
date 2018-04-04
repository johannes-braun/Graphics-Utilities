#pragma once

#include <vector>
#include <map>
#include <memory>
#include <io/window.hpp>
#include "ui.hpp"

namespace gfx::ui
{
    class window;

    enum window_state
    {
        WIN_STATE_DEFAULT = 0,
        WIN_STATE_MINIMIZED = 1,
    };

    struct window_info
    {
        int layer = 0;
        window_state state = WIN_STATE_DEFAULT;
    };

    class window_manager
    {
    public:
        window_manager(const std::shared_ptr<io::window>& base);
        void register_window(window* w);
        void unregister_window(window* w);
        void bring_to_front(window* w);
        window_info& info(window* w);
        window* front_window_at(float x, float y);

        io::window& get_window();
        //game::ui& get_ui();

    private:
        //game::ui _ui;
        std::shared_ptr<io::window> _base_window;
        std::map<window*, window_info> _windows;
    };
}