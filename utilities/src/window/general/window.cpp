#include "../window.hpp"

namespace wf
{
    native_handle window::get_native_handle() const noexcept
    {
        return _handle;
    }

    window::window(const std::string& title, int width, int height, int pos_x, int pos_y)
        : window(std::wstring(title.begin(), title.end()), width, height, pos_x, pos_y)
    {

    }

    window::window(window&& w) noexcept
    {
        operator=(std::forward<window&&>(w));
    }

    window& window::operator=(window&& w) noexcept
    {
        drop_callbacks = std::move(w.drop_callbacks);
        resize_callbacks = std::move(w.resize_callbacks);
        maximize_callbacks = std::move(w.maximize_callbacks);
        minimize_callbacks = std::move(w.minimize_callbacks);
        key_callbacks = std::move(w.key_callbacks);
        char_callbacks = std::move(w.char_callbacks);
        scroll_callbacks = std::move(w.scroll_callbacks);

        _handle = w._handle;
        w._handle = native_handle::null;
        _creation_time = w._creation_time;
        _last_update_time = w._last_update_time;
        _update_time = w._update_time;
        _closes = w._closes;
        _cursor_mode = w._cursor_mode;
        _last_cursor_position = w._last_cursor_position;
        _last_window_size = w._last_window_size;
        _last_window_position = w._last_window_position;
        _is_fullscreen = w._is_fullscreen;
        return *this;
    }

    bool window::is_fullscreen() const noexcept
    {
        return _is_fullscreen;
    }

    point window::get_cursor_delta() const noexcept
    {
        point p = cursor_position();
        return point{ p.x - _last_cursor_position.x, p.y - _last_cursor_position.y };
    }

    point window::cursor_position() const noexcept
    {
        rect rect = get_rect();
        point p = wf::cursor_position();
        p.x -= rect.left;
        p.y -= rect.top;
        return p;
    }

    void window::set_cursor_position(int x, int y) noexcept
    {
        set_cursor_position({ x, y });
    }

    void window::set_cursor_position(point p) noexcept
    {
        rect rect = get_rect();
        p.x -= rect.left;
        p.y -= rect.top;
        wf::set_cursor_position(p);
    }

    window::duration_type window::time_open() const noexcept
    {
        return std::chrono::system_clock::now() - _creation_time;
    }

    window::duration_type window::time_update() const noexcept
    {
        return _update_time - _last_update_time;
    }


    cursor_mode window::get_cursor_mode() const noexcept
    {
        return _cursor_mode;
    }

    void window::dispatch_drop(const files::path& path) const
    {
        for (const auto& callback : drop_callbacks)
            callback(path);
    }

    void window::dispatch_resize(int x, int y) const
    {
        for (const auto& callback : resize_callbacks)
            callback(x, y);
    }

    void window::dispatch_maximize(bool maximized) const
    {
        for (const auto& callback : maximize_callbacks)
            callback(maximized);
    }

    void window::dispatch_minimize(bool minimized) const
    {
        for (const auto& callback : minimize_callbacks)
            callback(minimized);
    }

    void window::dispatch_key(int key, bool down, bool alt) const
    {
        for (const auto& callback : key_callbacks)
            callback(key, down, alt);
    }

    void window::dispatch_char(wchar_t c) const
    {
        for (const auto& callback : char_callbacks)
            callback(c);
    }

    void window::dispatch_scroll(float x, float y) const
    {
        for (const auto& callback : scroll_callbacks)
            callback(x, y);
    }
}