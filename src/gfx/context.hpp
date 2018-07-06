#pragma once

#include "callbacks.hpp"
#include "graphics/swapchain.hpp"
#include <GLFW/glfw3.h>
#include <any>
#include <gfx/log.hpp>
#include <memory>
#include <optional>
#include <string_view>
#include "api.hpp"

namespace gfx
{
struct context_options
{
    gapi graphics_api = gapi::opengl;
    bool debug        = true;

    bool        use_window          = true;
    std::string window_title        = "";
    uint32_t    framebuffer_samples = 1;
    uint32_t    framebuffer_images  = 3;
    uint32_t    window_width        = 1280;
    uint32_t    window_height       = 720;
};

namespace detail
{
    class context_implementation
    {
    public:
        virtual ~context_implementation()             = default;
        virtual void initialize(GLFWwindow* window, const context_options& options)   = 0;
        virtual void make_current(GLFWwindow* window) = 0;
    };
    std::unique_ptr<context_implementation> make_context_implementation(gapi api);
} // namespace detail

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
    static inline std::shared_ptr<context> _null_context    = nullptr;
    static inline std::shared_ptr<context> _current_context = nullptr;

    context_options                                 _options;
    std::optional<gfx::swapchain>                   _swapchain;
    std::atomic_bool                                _should_close = false;
    double                                          _last_time;
    double                                          _delta = 0;
    GLFWwindow*                                     _window;
    std::unique_ptr<detail::context_implementation> _implementation;
};

} // namespace gfx
