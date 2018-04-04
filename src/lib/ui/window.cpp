#include "window.hpp"

namespace gfx::ui
{
    window::window(const std::shared_ptr<io::window>& window, rect initial_rect)
        : _list(window), _rect(initial_rect)
    {

    }

    const rect& window::get_rect() const noexcept
    {
        return _rect;
    }

    void window::fill(std::function<void(const window& window)> content)
    {
        
    }
}