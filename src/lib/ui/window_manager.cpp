#include "window_manager.hpp"
#include <cassert>
#include <algorithm>

namespace gfx::ui
{
    window_manager::window_manager(const std::shared_ptr<io::window>& base)
        : _base_window(base), _ui(*base)
    {

    }

    void window_manager::register_window(window* w)
    {
        if (_windows.count(w) != 0)
            return;
        window_info& inserted = _windows[w];
        inserted.layer = _windows.size()-1;
        inserted.state = WIN_STATE_DEFAULT;
    }

    void window_manager::unregister_window(window* w)
    {
        _windows.erase(w);
    }

    void window_manager::bring_to_front(window* w)
    {
        assert(_windows.count(w) != 0);

        const int previous_layer = _windows[w].layer;
        for (auto&& pair : _windows)
        {
            if (pair.first == w)
                pair.second.layer = _windows.size() - 1;
            else
                pair.second.layer = std::max(pair.second.layer - 1, 0);
        }
    }

    window_info& window_manager::info(window* w)
    {
        assert(_windows.count(w) != 0);
        return _windows[w];
    }

    window* window_manager::front_window_at(float x, float y)
    {
        int front_layer = -1;
        window* front_window = nullptr;
        for (const auto& pair : _windows)
        {
            if (pair.second.layer > front_layer)
            {
                front_window = pair.first;
                front_layer = pair.second.layer;
            }
        }
        return front_window;
    }

    io::window& window_manager::get_window()
    {
        return *_base_window;
    }

    game::ui& window_manager::get_ui()
    {
        return _ui;
    }
}