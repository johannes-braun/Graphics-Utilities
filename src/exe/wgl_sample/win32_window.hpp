#pragma once

#include "types.hpp"
#include "win32_internal.hpp"
#include "win32_monitor.hpp"

#include <memory>
#include <string>
#include <filesystem>
#include <functional>
#include <chrono>
#include <vector>

namespace winfw::platform::win32
{
    bool is_key_down(int key) noexcept;
    bool is_key_press(int key) noexcept;
    point cursor_position() noexcept;
    std::wstring get_clipboard_text() noexcept;
    void set_clipboard_text(const std::wstring& data) noexcept;

    class window
    {
        using handle = void*;
        struct message;
    public:
        using duration_type = std::chrono::duration<double>;
        using time_type = std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>>;

        window(const std::string& title, int width, int height, int pos_x = 0, int pos_y = 0);
        window(const std::wstring& title, int width, int height, int pos_x = 0, int pos_y = 0);
        ~window();

        bool poll();
        void close() noexcept;

        rect get_rect() const noexcept;
        point cursor_position() const noexcept;

        duration_type time_open() const noexcept;
        duration_type time_update() const noexcept;
        
        void set_cursor_mode(cursor_mode mode) noexcept;
        cursor_mode get_cursor_mode() const noexcept;
        point get_cursor_delta() const noexcept;

        monitor get_monitor() const noexcept;
        void fullscreen_monitor(const monitor& m) noexcept;
        void fullscreen_monitor(std::nullptr_t) noexcept;
        bool is_fullscreen() const noexcept;

        std::vector<std::function<drop_fun>> drop_callbacks;
        std::vector<std::function<resize_fun>> resize_callbacks;
        std::vector<std::function<maximize_fun>> maximize_callbacks;
        std::vector<std::function<minimize_fun>> minimize_callbacks;
        std::vector<std::function<key_fun>> key_callbacks;
        std::vector<std::function<char_fun>> char_callbacks;
        std::vector<std::function<scroll_fun>> scroll_callbacks;

    WINFW_CALLBACKS
        void on_drop(const files::path& path) const;
        void on_resize(int x, int y) const;
        void on_maximize(bool maximized) const;
        void on_minimize(bool minimized) const;
        void on_key(int key, bool down) const;
        void on_char(wchar_t c) const;
        void on_scroll(float x, float y) const;

    WINFW_HANDLES
        handle _handle;
        std::unique_ptr<message> _msg;

    private:
        std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>> _creation_time;
        std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>> _last_update_time;
        std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>> _update_time;
        bool _closes = false;
        cursor_mode _cursor_mode = cursor_mode::none;
        point _last_cursor_position{ 0, 0 };
        point _last_window_size{ 0, 0 };
        point _last_window_position{ 0, 0 };
        bool _is_fullscreen = false;
    };
}