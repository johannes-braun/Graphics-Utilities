#pragma once

#include "api.hpp"
#include "callbacks.hpp"
#include "graphics/swapchain.hpp"
#include <GLFW/glfw3.h>
#include <any>
#include <gfx/log.hpp>
#include <memory>
#include <optional>
#include <string_view>

namespace gfx {
inline namespace v1 {
struct context_options
{
    gapi graphics_api = gapi::opengl;
    bool debug        = true;

    bool         use_window          = true;
    std::string  window_title        = "";
    sample_count framebuffer_samples = sample_count::x1;
    u32          framebuffer_images  = 3;
    u32          window_width        = 1280;
    u32          window_height       = 720;
    u32          max_framerate       = 120;
};

namespace detail {
class context_implementation
{
public:
    virtual ~context_implementation()                                           = default;
    virtual void initialize(GLFWwindow* window, const context_options& options) = 0;
    virtual void make_current(GLFWwindow* window)                               = 0;
    virtual bool on_run(bool open) { return open; }
};
std::unique_ptr<context_implementation> make_context_implementation(gapi api);
}    // namespace detail

class context : public std::enable_shared_from_this<context>, public callbacks
{
public:
    static std::shared_ptr<context>  create(const context_options& options);
    static std::shared_ptr<context>& current();
    static void                      make_current(const std::shared_ptr<context>& ctx);

    ~context();
    void make_current();

    GLFWwindow*            window() const noexcept;
    const context_options& options() const noexcept;

    std::any implementation() const noexcept;
    bool     should_close() const;
    bool     run();
    double   delta() const noexcept;

    const std::optional<gfx::swapchain>& swapchain() const noexcept { return _swapchain; }

private:
    static inline struct glfw
    {
        glfw();
        ~glfw();
    } glfw_init;

    context(const context_options& options);
    static inline std::shared_ptr<context> _null_context = nullptr;

    std::unique_ptr<detail::context_implementation> _implementation;
    context_options                                 _options;
    std::optional<gfx::swapchain>                   _swapchain;
    std::atomic_bool                                _should_close = false;
    double                                          _last_time;
    double                                          _delta = 0;
    GLFWwindow*                                     _window;
};

}    // namespace v1
}    // namespace gfx
