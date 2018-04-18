#include "../../window.hpp"
#include "../../log.hpp"
#include <cassert>

namespace gfx
{
    window::window(const std::string& api, const std::string_view title, const uint32_t width, const uint32_t height, const window_hints& hints) noexcept
        : window(api, title, width, height, nullptr, hints)
    {

    }

    window::window(const std::string& api, const std::string_view title, const uint32_t width, const uint32_t height, const std::shared_ptr<window>& share, const window_hints& hints) noexcept
        : _share(share), _api(api)
    {
        [[maybe_unused]] static struct glfw { glfw() { glfwInit(); } ~glfw() { glfwTerminate(); } } glfw_init;

        for (const auto& h : hints)
            glfwWindowHint(h.first, h.second);

        glfwWindowHint(GLFW_CLIENT_API, int(api_manager::info(_api).api_type));

        glfwSetErrorCallback([](int code, const char* msg) { tlog_e("GLFW Callback") << "[ Code: " << code << " ]: " << msg; });
        _window = std::unique_ptr<GLFWwindow, glfw_delete_window>(glfwCreateWindow(int(width), int(height), title.data(), nullptr, _share ? _share->_window.get() : nullptr));
        init(_window.get());

        _handler = api_manager::info(_api).create_window(*this);
        if (!_handler)
            log_e << "Invalid API name: " << _api;
    }

    window::operator GLFWwindow*() const noexcept
    {
        return _window.get();
    }

    void window::set_icon(const image_file& image) const noexcept
    {
        assert(image.channel_bits == gfx::bits::b8 && "Invalid icon format.");
        GLFWimage icon_image;
        icon_image.width = image.width;
        icon_image.height = image.height;
        icon_image.pixels = static_cast<unsigned char*>(image.bytes());
        glfwSetWindowIcon(_window.get(), 1, reinterpret_cast<const GLFWimage*>(&icon_image));
    }

    void window::set_swap_delay(const double delay) noexcept
    {
        _frame_swap_delay = delay;
    }

    void window::set_max_framerate(const double fps) noexcept
    {
        _frame_swap_delay = 1/fps;
    }

    double window::swap_delay() const noexcept
    {
        return _frame_swap_delay;
    }

    void window::close() const noexcept
    {
        glfwSetWindowShouldClose(_window.get(), true);
    }

    bool window::update()
    {
        glfwPollEvents();

        int ww, wh;
        glfwGetFramebufferSize(_window.get(), &ww, &wh);
        while (ww*wh == 0)
        {
            glfwPollEvents();
            glfwGetFramebufferSize(_window.get(), &ww, &wh);
        }

        _handler->do_update();

        while (_frame_begin_time > glfwGetTime() - _frame_swap_delay)
            glfwPollEvents();
        _frame_delta_time = (glfwGetTime() - _frame_begin_time);
        _frame_begin_time = glfwGetTime();

        const bool is_open = !glfwWindowShouldClose(_window.get());
        return is_open;
    }

    double window::delta_time() const noexcept
    {
        return _frame_delta_time;
    }

    const std::string& window::context_api() const noexcept
    {
        return _api;
    }

    rect2f window::get_rect() const noexcept
    {
        int w, h, x, y;
        glfwGetFramebufferSize(*this, &w, &h);
        glfwGetWindowPos(*this, &x, &y);
        return rect2f({ float(x), float(y) }, { float(x+w), float(y+h) });
    }
}
