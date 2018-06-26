#include "graphics/opengl/context_opengl.hpp"
#include <gfx/context.hpp>
//#include "context_vulkan.hpp"

namespace gfx
{
namespace detail
{
    std::unique_ptr<context_implementation> make_context_implementation(const gapi api)
    {
        switch(api)
        {
        case gapi::opengl:
            return std::make_unique<opengl::context_implementation>();
        case gapi::vulkan:
            // return std::make_unique<vulkan::context_implementation>();
        default:
            break;
        }
        return nullptr;
    }
}

std::shared_ptr<context> context::create(const context_options& options)
{
    return std::shared_ptr<context>(new context(options));
}
std::shared_ptr<context>& context::current()
{
    return _current_context ? _current_context : _null_context;
}
void context::make_current(const std::shared_ptr<context>& ctx) { _current_context = ctx; }

context::~context()
{
    if(_current_context && _current_context.get() == this)
        _current_context = nullptr;

    glfwDestroyWindow(_window);
}

void context::make_current()
{
    _current_context = shared_from_this();
    _implementation->make_current(_window);
}

GLFWwindow* context::window() const noexcept { return _window; }

std::any context::implementation() const noexcept { return &*_implementation; }

bool context::should_close() { return _should_close; }
bool context::run()
{
    make_current();

    if(_swapchain)
        _swapchain->present();
    if(!_swapchain && _options.use_window)
        _swapchain.emplace();
    _should_close = glfwWindowShouldClose(_window);
    glfwPollEvents();

    double t   = glfwGetTime();
    _delta     = t - _last_time;
    _last_time = glfwGetTime();
    return !should_close();
}

double context::delta() const noexcept { return _delta; }

context::glfw::glfw()
{
    glfwSetErrorCallback([](int, const char* m) { gfx::dlog << m; });
    glfwInit();
}
context::glfw::~glfw() { glfwTerminate(); }

const context_options& context::options() const noexcept { return _options; }

context::context(const context_options& options)
        : _options(options)
{
    glfwWindowHint(GLFW_CLIENT_API,
                   _options.graphics_api == gapi::opengl ? GLFW_OPENGL_API : GLFW_NO_API);
    glfwWindowHint(GLFW_SAMPLES, _options.framebuffer_samples);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, _options.debug);

    _last_time = glfwGetTime();
    _window    = glfwCreateWindow(options.window_width,
                               options.window_height,
                               options.window_title.c_str(),
                               nullptr,
                               (current() && current()->options().graphics_api == gapi::opengl)
                                       ? current()->_window
                                       : nullptr);
    framebuffer_size_callback.add([](GLFWwindow* win, int w, int h) {
        auto& c                  = *reinterpret_cast<context*>(glfwGetWindowUserPointer(win));
        c._options.window_width  = w; 
        c._options.window_height = h;
        if(c._swapchain)
            c._swapchain.value().resize(w, h);

    });
    glfwSetWindowUserPointer(_window, this);
    _implementation = detail::make_context_implementation(_options.graphics_api);
    _implementation->initialize(_window);
    callbacks::init(_window);
    glfwSwapInterval(0);
}
}
