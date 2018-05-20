#include "window.hpp"
#include "window_manager.hpp"
#include <gfx/file.hpp>

namespace gfx::ui
{
    window::window(const std::shared_ptr<gfx::window>& window, window_manager* wm, const std::wstring& title, rect initial_rect)
        : _window_manager(wm), _window(window), _list(window), _title(title), _rect(initial_rect)
    {

    }

    const rect& window::get_rect() const noexcept
    {
        return _rect;
    }

    rect window::get_extended_rect() const noexcept
    {
        rect inset = _rect;
        return inset.inset(-_grip_size, -_grip_size, -_grip_size, -_grip_size);
    }

    rect window::get_content_rect() const noexcept
    {
        rect inset = _rect;
        return inset.inset(0, _title_bar_size, 0, 0);
    }

    void window::set_contains_cursor(bool contains)
    {
        _contains_cursor = contains;
    }

    void window::fill(const font& title_font, std::function<void(window& window)> content)
    {
        static const auto load_svg = [](const auto& path, float scale = 1.f) {
            const gfx::image_file img(path, scale);
            gl::texture ico(GL_TEXTURE_2D, img.width, img.height, GL_RGBA8, 1);
            ico.assign(GL_RGBA, GL_UNSIGNED_BYTE, img.bytes());
            return ico;
        };
        static gl::texture close_icon = load_svg("../res/ui/icons/ic_close_white_24px.svg");
        static gl::texture minimize_icon = load_svg("../res/ui/icons/ic_remove_white_24px.svg");
        static gl::texture deminimize_icon = load_svg("../res/ui/icons/ic_expand_less_white_24px.svg");
        static gl::texture maximize_icon = load_svg("../res/ui/icons/ic_fullscreen_white_24px.svg");
        static gl::texture demaximize_icon = load_svg("../res/ui/icons/ic_fullscreen_exit_white_24px.svg");
        static gl::texture default_icon = load_svg("../res/ui/logo.svg", 0.75f);

        handle_animations();
        const glm::vec2 shadow_size(6, 6);
        const glm::vec2 shadow_offset(0, -1);
        const glm::vec2 pos = _rect.min;
        const glm::vec2 size = _rect.max - _rect.min;

        _list.push_rounded_quad(_rect.min-shadow_size+shadow_offset, _rect.max+shadow_size+shadow_offset, { 0, 1 }, { 1, 0 }, { 0, 0, 0, 60*_shadow_alpha }, { 0, 0, 0, 0 }, 12, gfx::ui::CORNER_ALL);
        _list.push_scissor(pos.x, pos.y, size.x, size.y);
        if(size.y > _title_bar_size)
            _list.push_rounded_quad(_rect.min, _rect.max, { 0, 1 }, { 1, 0 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, 4.f, gfx::ui::CORNER_ALL);

        _list.push_rounded_quad({ _rect.min.x, _rect.max.y - _title_bar_size }, _rect.max, { 0, 1 }, { 1, 0 },
            { 0x00, 0x68, 0x3b, 255 }, { 0x00, 0x68, 0x3b, 255 }, _title_bar_rounding, gfx::ui::CORNER_TOP_LEFT | gfx::ui::CORNER_TOP_RIGHT);

        _list.push_quad({ _rect.min.x + 8, _rect.max.y - 0.5f*(_title_bar_size + 24) }, { _rect.min.x + 8 + 24, _rect.max.y - 0.5f*(_title_bar_size - 24) }, { 0, 1 }, { 1, 0 }, { 255, 255, 255, 255 }, {},
            GRADIENT_NONE, default_icon);
        _list.push_text(_title, title_font, 0, { _rect.min.x + 8 + 24 + 8, _rect.max.y - _title_bar_size },
            { _rect.max.x - 8 - 24-8-24-8-24-8, _rect.max.y - 0.5f*(_title_bar_size-title_font.size()) }, gfx::ui::ALIGN_LEFT, {255, 255, 255, 255*_title_alpha}, 1);

        const auto make_action_button = [&, this](mygl::texture img, rect r) {
            const auto cur_pos = _window_manager->get_cursor_position();
            bool active = _window_manager->front_window_at(cur_pos.x, cur_pos.y) == this;
            bool inside = r.contains(_window_manager->get_cursor_position());

            float off = 0.f;
            if (active && inside && _window_manager->get_mouse_left_state() == MOUSE_UP)
                off = 4.f;
            else if(active && inside && _window_manager->get_mouse_left_state() == MOUSE_DOWN)
                off = 6.f;

            _list.push_quad(r.min + off, r.max - off, { 0, 1 }, { 1, 0 }, { 255, 255, 255, _actions_alpha*255 }, {}, gfx::ui::GRADIENT_NONE, img);
            return active && inside && _window_manager->get_mouse_left_state() == MOUSE_PRESS;
        };

        if(make_action_button(close_icon, { { _rect.max.x - 8 - 24, _rect.max.y - 0.5f*(_title_bar_size + 24) }, { _rect.max.x - 8, _rect.max.y - 0.5f*(_title_bar_size - 24) } }))
        {
            animate<glm::vec2>().to_size({0, 0}).to_position(_window_manager->get_cursor_position());
            _1f_animations.push_back(anim<float>(_shadow_alpha, 0.f, 0.080, [this](float a, float x) { _shadow_alpha = x; }));
            _1f_animations.push_back(anim<float>(_actions_alpha, 0.f, 0.080, [this](float a, float x) { _actions_alpha = x; }));
            _1f_animations.push_back(anim<float>(_title_alpha, 0.f, 0.080, [this](float a, float x) { _title_alpha = x; }));
            _window_manager->notify_unminimize(this);
        }
        if (make_action_button(_state==WIN_MAXIMIZED ? demaximize_icon : maximize_icon, { { _rect.max.x - 8 - 24-8-24, _rect.max.y - 0.5f*(_title_bar_size + 24) }, { _rect.max.x - 8 - 24-8, _rect.max.y - 0.5f*(_title_bar_size - 24) } }))
        {
            if (_state == WIN_NORMAL || _state == WIN_MINIMIZED)
            {
                const rect estate = _window_manager->get_maximal_rect();
                animate<glm::vec2>().to_size(estate.max - estate.min).to_position(estate.min);
                if (_state == WIN_NORMAL) _default_rect = _rect;
                else {
                    _1f_animations.push_back(anim<float>(_title_alpha, 1.f, 0.080, [this](float a, float x) { _title_alpha = x; }));
                    _1f_animations.push_back(anim<float>(_title_bar_rounding, 4.f, 0.080, [this](float a, float x) { _title_bar_rounding = x; }));
                    _1f_animations.push_back(anim<float>(_shadow_alpha, 1.f, 0.080, [this](float a, float x) { _shadow_alpha = x; }));
                }
                _state = WIN_MAXIMIZED;
                _window_manager->notify_unminimize(this);
            }
            else
            {
                animate<glm::vec2>().to_size(_default_rect.max - _default_rect.min).to_position(_default_rect.min);
                _state = WIN_NORMAL;
            }
        }
        if (make_action_button(_state == WIN_MINIMIZED ? deminimize_icon : minimize_icon, { { _rect.max.x - 8 - 24-8-24-8-24, _rect.max.y - 0.5f*(_title_bar_size + 24) },{ _rect.max.x - 8 - 24-8-24-8, _rect.max.y - 0.5f*(_title_bar_size - 24) } }))
        {
            if (_state == WIN_NORMAL || _state == WIN_MAXIMIZED)
            {
                _1f_animations.push_back(anim<float>(_title_alpha, 0.f, 0.080, [this](float a, float x) { _title_alpha = x; }));
                _1f_animations.push_back(anim<float>(_title_bar_rounding, 0.f, 0.080, [this](float a, float x) { _title_bar_rounding = x; }));
                _1f_animations.push_back(anim<float>(_shadow_alpha, 0.f, 0.080, [this](float a, float x) { _shadow_alpha = x; }));
                animate<glm::vec2>().to_position(_window_manager->minimize_position()).to_size({ 104+8+24, _title_bar_size });
                if (_state == WIN_NORMAL) _default_rect = _rect;
                _state = WIN_MINIMIZED;
                _window_manager->notify_minimize(this);
            }
            else
            {
                _1f_animations.push_back(anim<float>(_title_alpha, 1.f, 0.080, [this](float a, float x) { _title_alpha = x; }));
                _1f_animations.push_back(anim<float>(_title_bar_rounding, 4.f, 0.080, [this](float a, float x) { _title_bar_rounding = x; }));
                _1f_animations.push_back(anim<float>(_shadow_alpha, 1.f, 0.080, [this](float a, float x) { _shadow_alpha = x; }));
                animate<glm::vec2>().to_size(_default_rect.max - _default_rect.min).to_position(_default_rect.min);
                _state = WIN_NORMAL;
                _window_manager->notify_unminimize(this);
            }
        }

        if (size.y > _title_bar_size)
            content(*this);
    }

    win_state window::state() const noexcept
    {
        return _state;
    }

    void window::move_by(glm::vec2 by)
    {
        _rect.min += by;
        _rect.max += by;
    }

    void window::resize_by(glm::vec2 by)
    {
        _rect.max += by;
    }

    rect window::title_area() const noexcept
    {
        return { { _rect.min.x, _rect.max.y - _title_bar_size}, { _rect.max.x, _rect.max.y} };
    }

    rect window::grip_left() const noexcept
    {
        return { { _rect.min.x-_grip_size, _rect.min.y }, {_rect.min.x + _grip_size, _rect.max.y} };
    }

    rect window::grip_right() const noexcept
    {
        return { { _rect.max.x-_grip_size, _rect.min.y },{ _rect.max.x+_grip_size, _rect.max.y } };
    }

    rect window::grip_top() const noexcept
    {
        return { { _rect.min.x, _rect.max.y-_grip_size },{ _rect.max.x, _rect.max.y+_grip_size } };
    }

    rect window::grip_bottom() const noexcept
    {
        return { { _rect.min.x, _rect.min.y-_grip_size },{ _rect.max.x, _rect.min.y+_grip_size } };
    }

    draw_list& window::list()
    {
        return _list;
    }

    void window::handle_animations() noexcept
    {
        const auto handle_all = [this](auto& vec)
        {
            for (auto it = vec.begin(); it != vec.end();)
            {
                it->update(_window->delta_time());
                if (it->finished())
                {
                    it->update(1);
                    it = vec.erase(it);
                }
                else
                    ++it;
            }
        };
        handle_all(_1f_animations);
        handle_all(_2f_animations);
    }
}