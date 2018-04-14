#include "../../window.hpp"
#include "../../log.hpp"

namespace gfx
{
    window::window(const api api, const std::string_view title, const uint32_t width, const uint32_t height) noexcept
        : window(api, title, width, height, nullptr)
    {

    }

    window::window(const api api, const std::string_view title, const uint32_t width, const uint32_t height, const std::shared_ptr<window>& share) noexcept
        : _share(share), _api(api)
    {
       [[maybe_unused]] static struct glfw { glfw() { glfwInit(); } ~glfw() { glfwTerminate(); } } glfw_init;

        switch (_api)
        {
        case api::opengl: glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API); break;
        case api::vulkan: glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);     break;
        }

        glfwSetErrorCallback([](int code, const char* msg) { tlog_e("GLFW Callback") << "[ Code: " << code << " ]: " << msg; });
        _window = std::unique_ptr<GLFWwindow, glfw_delete_window>(glfwCreateWindow(int(width), int(height), title.data(), nullptr, _share ? _share->_window.get() : nullptr));
        init(_window.get());

        switch(_api)
        {
        case api::opengl: _handler = std::make_unique<window_handler_opengl>(*this); break;
        case api::vulkan: _handler = std::make_unique<window_handler_vulkan>(*this); break;
        }
    }

    window::operator GLFWwindow*() const noexcept
    {
        return _window.get();
    }

    void window::set_icon(const gfx::image_file& image) const noexcept
    {
        assert(image.channel_bits == gfx::bits::b8 && "Invalid icon format.");
        GLFWimage icon_image{};
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

    api window::context_api() const noexcept
    {
        return _api;
    }
}