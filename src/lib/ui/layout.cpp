#include "layout.hpp"
#include "window.hpp"
#include "window_manager.hpp"

namespace gfx::ui
{
    void layout::fill(std::function<void(layout& layout, window& parent)> fill_func)
    {
        const rect parent_rect = _parent->get_content_rect();
        _parent->list().push_scissor(parent_rect.min.x, parent_rect.min.y, parent_rect.max.x - parent_rect.min.x, parent_rect.max.y - parent_rect.min.y);
       // _parent->list().push_scissor(_rect.min.x, _rect.min.y, _rect.max.x - _rect.min.x, _rect.max.y - _rect.min.y);
        fill_func(*this, *_parent);
        _parent->list().push_scissor(parent_rect.min.x, parent_rect.min.y, parent_rect.max.x - parent_rect.min.x, parent_rect.max.y - parent_rect.min.y);
        update();
    }

    layout::layout(window* parent, const rect& area)
        : _parent(parent), _rect(area)
    {

    }

    rect layout::get_rect() const noexcept
    {
        return _rect;
    }

    rect layout::get_clip_rect() const noexcept
    {
        return _rect;
    }

    scroll_layout::~scroll_layout()
    {
        _parent->_window->callbacks->scroll_callback.remove(_callback);
    }

    scroll_layout::scroll_layout(window* parent, const rect& area)
        : layout(parent, area), _scroll(0), _smooth_scroll(_scroll - area.max.y - (area.max.y - area.min.y)), _scroll_rect(area), _last_scroll_rect(area),
        _callback(_parent->_window->callbacks->scroll_callback.add([this](GLFWwindow* w, double x, double y) {

        const auto cur_pos = _parent->_window_manager->get_cursor_position();
        bool active = _parent->_window_manager->front_window_at(cur_pos.x, cur_pos.y) == _parent;
        bool inside = _rect.contains(_parent->_window_manager->get_cursor_position());
        if(active&&inside) _scroll -= 40*y;
    }))
    {}

    void scroll_layout::force_layout(const rect& new_area)
    {
        _rect = new_area;
        const float size = _scroll_rect.max.y - _scroll_rect.min.y;
        const float clip_size = _rect.max.y - _rect.min.y;
        _scroll_rect.min.x = _last_scroll_rect.min.x = _rect.min.x;
        _scroll_rect.max.x = _last_scroll_rect.max.x = _rect.max.x;
        _scroll_rect.max.y = _last_scroll_rect.max.y = _rect.max.y;
        _scroll = std::clamp(_scroll, 0.f, std::clamp(size - clip_size, 0.f, size));
        _smooth_scroll = _scroll + _rect.max.y;
        _scroll_rect.max.y = _smooth_scroll;
        _scroll_rect.min.y = _scroll_rect.max.y - size;
    }

    void scroll_layout::update()
    {
        _scroll_rect.min.x = _last_scroll_rect.min.x = _rect.min.x;
        _scroll_rect.max.x = _last_scroll_rect.max.x = _rect.max.x;
        _scroll_rect.max.y = _last_scroll_rect.max.y = _rect.max.y;
        _scroll_rect = _last_scroll_rect;
        _last_scroll_rect.min.y = _last_scroll_rect.max.y;
        const float size = _scroll_rect.max.y - _scroll_rect.min.y;
        const float clip_size = _rect.max.y - _rect.min.y;
        _scroll = std::clamp(_scroll, 0.f, std::clamp(size - clip_size, 0.f, size));
        _smooth_scroll = glm::mix(_smooth_scroll, _scroll + _rect.max.y, float(20 * _parent->_window->delta_time()));
        _scroll_rect.max.y = _smooth_scroll;
        _scroll_rect.min.y = _scroll_rect.max.y - size;
    }

    void scroll_layout::add_content_height(float h) noexcept
    {
        _last_scroll_rect.min.y -= h;
    }

    rect scroll_layout::get_rect() const noexcept
    {
        return _scroll_rect;
    }

    rect linear_layout::get_rect() const noexcept
    {
        return _layout_rect;
    }

    void linear_layout::add_content_height(float h) noexcept
    {
        _layout_rect.max.y -= h;
    }

    void linear_layout::force_layout(const rect& new_area)
    {
        _layout_rect = _rect = new_area;
    }

    linear_layout::linear_layout(window* parent, const rect& area)
        : layout(parent, area), _layout_rect(area)
    {

    }

    void linear_layout::update()
    {
        _layout_rect = _rect;
    }
}