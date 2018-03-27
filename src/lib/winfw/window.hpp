#pragma once

#include "types.hpp"
#include "monitor.hpp"

#include <memory>
#include <string>
#include <filesystem>
#include <functional>
#include <chrono>
#include <vector>

#if __has_include(<vulkan/vulkan.hpp>)
#include <vulkan/vulkan.hpp>
#endif

namespace winfw
{
    bool is_key_down(int key) noexcept;
    bool is_key_press(int key) noexcept;
    point cursor_position() noexcept;
    std::wstring get_clipboard_text() noexcept;
    void set_clipboard_text(const std::wstring& data) noexcept;

    class window
    {
    public:
        using duration_type = std::chrono::duration<double>;
        using time_type = std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>>;

        // Calls the wstring constructor variant after converting the string to a wstring.
        window(const std::string& title, int width, int height, int pos_x = 0, int pos_y = 0);
        window(const std::wstring& title, int width, int height, int pos_x = 0, int pos_y = 0);
        ~window();

        // Updates the native window message queue.
        // Returns whether the window is opened. (example: while(window.poll()) { ... })
        bool poll();
        // Sets a hint for the window to be closed. Will later be returned by poll() as false.
        void close() noexcept;

        // Window properties

        // Returns the window client area (without window decoration).
        rect get_rect() const noexcept;
        // Returns the cursor position relative to the top left corner of the client area.
        point cursor_position() const noexcept;
        // Returns a universal handle which must be casted to a platform-dependant window handle for further access.
        native_handle get_native_handle() const noexcept;


        // Time settings

        // Returns the time which has passed since opening the window.
        duration_type time_open() const noexcept;
        // Returns the time between the last two calls to window::poll().
        duration_type time_update() const noexcept;
        

        // Cursor settings

        // Set the cursor mode to the following modes:
        // 1) cursor_mode::none to keep the cursor free from any bounds.
        // 2) cursor_mode::captured to constraint the cursor to inside the window client bounds.
        // 3) cursor_mode::center_captured to hide the cursor and keep it locked at the center.
        void set_cursor_mode(cursor_mode mode) noexcept;
        cursor_mode get_cursor_mode() const noexcept;
        // Returns the distance the cursor has travelled since the last call to poll().
        point get_cursor_delta() const noexcept;


        // Monitor settings

        // Returns the monitor nearest to the window position.
        monitor get_monitor() const noexcept;
        // Set a monitor on which the window should be set to fullscreen.
        void fullscreen_monitor(const monitor& m) noexcept;
        // Resets the monitor to its default non-fullscreen state.
        void fullscreen_monitor(std::nullptr_t) noexcept;
        // Returns true, if the window currently is in fullscreen-mode, false otherwise.
        bool is_fullscreen() const noexcept;
        

        // Callbacks

        // Called when one or more files are dropped into the window client area.
        std::vector<std::function<drop_fun>> drop_callbacks;
        // Called when the window has been resized.
        std::vector<std::function<resize_fun>> resize_callbacks;
        // Called when the window has been maximized.
        std::vector<std::function<maximize_fun>> maximize_callbacks;
        // Called when the window has been minimized.
        std::vector<std::function<minimize_fun>> minimize_callbacks;
        // Called when a key/button has been pressed or released.
        std::vector<std::function<key_fun>> key_callbacks;
        // Called when a character key has been pressed or released.
        std::vector<std::function<char_fun>> char_callbacks;
        // Called when there was a vertical or horizontal scroll event.
        std::vector<std::function<scroll_fun>> scroll_callbacks;

        void dispatch_drop(const files::path& path) const;
        void dispatch_resize(int x, int y) const;
        void dispatch_maximize(bool maximized) const;
        void dispatch_minimize(bool minimized) const;
        void dispatch_key(int key, bool down) const;
        void dispatch_char(wchar_t c) const;
        void dispatch_scroll(float x, float y) const;


#if __has_include(<vulkan/vulkan.hpp>)
        vk::SurfaceKHR create_surface(vk::Instance instance, vk::Optional<const vk::AllocationCallbacks> callbacks = nullptr) const noexcept;
#endif

    private:
        native_handle _handle;
        time_type _creation_time;
        time_type _last_update_time;
        time_type _update_time;
        bool _closes = false;
        cursor_mode _cursor_mode = cursor_mode::none;
        point _last_cursor_position{ 0, 0 };
        point _last_window_size{ 0, 0 };
        point _last_window_position{ 0, 0 };
        bool _is_fullscreen = false;
    };
}