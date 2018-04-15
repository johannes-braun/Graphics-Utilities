#include "../window_handler.hpp"

namespace gfx
{
    window_handler::window_handler(window& w)
        : _window(w) {}

    window& window_handler::get_window() const noexcept
    {
        return _window;
    }
}