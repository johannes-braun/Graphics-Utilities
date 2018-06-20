#pragma once

#include <GLFW/glfw3.h>
#include <any>
#include <gfx/log.hpp>
#include <memory>
#include <string_view>

namespace gfx
{
enum class gapi
{
    opengl = 0,
    vulkan
};

namespace detail
{
    class context_implementation
    {
    public:
        virtual ~context_implementation()             = default;
        virtual void initialize(GLFWwindow* window)   = 0;
        virtual void make_current(GLFWwindow* window) = 0;
    };
    std::unique_ptr<context_implementation> make_context_implementation(gapi api);
} // namespace detail

class context : public std::enable_shared_from_this<context>
{
    friend std::shared_ptr<context> std::make_shared<context>();

public:
    static std::shared_ptr<context> create(const gapi graphics_api, const std::string& name,
                                           uint32_t width, uint32_t height)
    {
        return std::shared_ptr<context>(new context(graphics_api, name, width, height));
    }
    static std::shared_ptr<context>& current()
    {
        return _current_context ? _current_context : _null_context;
    }
    static void make_current(const std::shared_ptr<context>& ctx) { _current_context = ctx; }

    ~context()
    {
        if(_current_context && _current_context.get() == this)
            _current_context = nullptr;

        glfwDestroyWindow(_window);
    }

    void make_current()
    {
        _current_context = shared_from_this();
        _implementation->make_current(_window);
    }

    GLFWwindow* window() const noexcept { return _window; }
    const gapi  graphics_api;

    std::any implementation() const noexcept { return &*_implementation; }

    template <typename Func, typename = decltype(std::declval<Func>()(double()))>
    void do_on_update(Func&& func)
    {
        _on_update  = func;
    }

    bool should_close() { return _should_close; }
    bool run() {
        make_current();
        double t     = glfwGetTime();
        double delta = t - _last_time;
        _last_time   = glfwGetTime();
        _on_update(delta);
        _should_close = glfwWindowShouldClose(_window);
        glfwPollEvents();
        glfwMakeContextCurrent(nullptr);
        return !should_close();
    }

private:
    static inline struct glfw
    {
        glfw() { glfwInit(); }
        ~glfw() { glfwTerminate(); }
    } glfw_init;

    context(const gapi graphics_api, const std::string& name, uint32_t width, uint32_t height)
            : graphics_api(graphics_api)
    {
        glfwWindowHint(GLFW_CLIENT_API,
                       graphics_api == gapi::opengl ? GLFW_OPENGL_API : GLFW_NO_API);
        glfwSetErrorCallback([](int, const char* m) { gfx::clogd << m; });

        _last_time = glfwGetTime();
        // std::atomic_bool created = false;
        //_window_thread = std::thread([&, this]() {
        _window         = glfwCreateWindow(width,
                                   height,
                                   name.c_str(),
                                   nullptr,
                                   (current() && current()->graphics_api == gapi::opengl)
                                           ? current()->_window
                                           : nullptr);
        _implementation = detail::make_context_implementation(graphics_api);
        _implementation->initialize(_window);
        glfwSwapInterval(0);

        //    created = true;

        //    while(!_has_update)
        //        ;
        //});

        ////Await creation
        // while(!created)
        //    ;
    }

    static inline std::shared_ptr<context> _null_context    = nullptr;
    static inline std::shared_ptr<context> _current_context = nullptr;

    std::atomic_bool                                _should_close = false;
    std::function<void(double)>                     _on_update;
    double                                          _last_time;
    GLFWwindow*                                     _window;
    std::unique_ptr<detail::context_implementation> _implementation;
};

} // namespace gfx
