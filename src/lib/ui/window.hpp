#pragma once

#include <string>
#include "draw_list.hpp"
#include "font.hpp"
#include "layout.hpp"
#include "rect.hpp"

namespace gfx::ui
{
    class window_manager;

    enum win_state
    {
        WIN_NORMAL = 0,
        WIN_MINIMIZED = 1,
        WIN_MAXIMIZED = 2
    };

    constexpr float default_title_bar_size = 32;
    constexpr float default_grip_size = 4.f;

    class window
    {
    public:
        friend layout;
        friend scroll_layout;
        template<typename T> class anim;
        template<typename T> class anim_creator;

        window(const std::shared_ptr<io::window>& window, window_manager* wm, const std::wstring& title, rect initial_rect);
        const rect& get_rect() const noexcept;
        rect get_extended_rect() const noexcept;
        rect get_content_rect() const noexcept;
        void fill(const font& title_font, std::function<void(window& window)> content);

        void move_by(glm::vec2 by);
        void resize_by(glm::vec2 by);
        void set_contains_cursor(bool contains);

        rect title_area() const noexcept;
        rect grip_left() const noexcept;
        rect grip_right() const noexcept;
        rect grip_top() const noexcept;
        rect grip_bottom() const noexcept;

        draw_list& list();
        win_state state() const noexcept;

        template<typename T>
        anim_creator<T> animate() { return anim_creator<T>{*this}; }

        template<typename T, typename = std::enable_if_t<std::is_base_of_v<layout, T>>>
        T& in_layout(const std::string& id, rect area)
        {
            if (_layouts.count(id) == 0)
            {
                _layouts.emplace(id, std::move(std::unique_ptr<T>(new T(this, area))));
                _layouts.at(id)->force_layout(area);
            }
            else if (_layouts.at(id)->get_clip_rect() != area)
                _layouts.at(id)->force_layout(area);
            return static_cast<T&>(*_layouts[id]);
        }

    private:
        void handle_animations() noexcept;

        win_state _state = WIN_NORMAL;

        std::unordered_map<std::string, std::unique_ptr<layout>> _layouts;
        window_manager* _window_manager;
        std::shared_ptr<io::window> _window;
        std::vector<anim<glm::vec2>> _2f_animations;
        std::vector<anim<float>> _1f_animations;
        std::wstring _title;
        draw_list _list;
        rect _rect;
        rect _default_rect;
        float _title_bar_size = default_title_bar_size;
        float _grip_size = default_grip_size;

        float _shadow_alpha = 1.f;
        float _title_alpha = 1.f;
        float _actions_alpha = 1.f;
        float _title_bar_rounding = 4.f;

        bool _contains_cursor = false;
    };

    template<typename T>
    class window::anim
    {
    public:
        friend anim_creator;
        anim(T begin, T end, double duration, std::function<void(float, const T&)> on_update) : begin(begin), end(end), duration(duration), alpha(0), on_update(on_update) {}
        void update(double dt) { on_update(alpha=std::clamp(alpha + dt/duration, 0.0, 1.0), (1-alpha) * begin + alpha * end); }
        bool finished() { return alpha == 1.0; }

    private:
        T begin;
        T end;
        float alpha;
        double duration;
        std::function<void(float a, const T& val)> on_update;
    };

    template<>
    class window::anim_creator<glm::vec2>
    {
    public:
        friend window;
        anim_creator& to_position(glm::vec2 position)
        {
            _w._2f_animations.emplace_back(_w._rect.min, position, 0.120, [win = &_w](float a, const glm::vec2& val) {
                const glm::vec2 size = win->_rect.max-win->_rect.min;
                win->_rect.min = val;
                win->_rect.max = win->_rect.min + size;
            });
            return *this;
        }
        anim_creator& to_size(glm::vec2 size) {
            _w._2f_animations.emplace_back(_w._rect.max - _w._rect.min, size, 0.120, [win = &_w](float a, const glm::vec2& val) {
                win->_rect.max = win->_rect.min + val;
            });
            return *this;
        }

    private:
        anim_creator(window& w) : _w(w) {}
        window& _w;
    };
}