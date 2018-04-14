#pragma once

#include <functional>
#include <algorithm>
#include <GLFW/glfw3.h>

namespace gfx
{
    
    template<typename T>
    class callback
    {
    public:
        using glfw_fun_ptr = std::function<std::remove_pointer_t<T>>;

        callback() = default;
        callback(const callback& other) = delete;
        callback(callback&& other) noexcept = delete;
        callback& operator=(const callback& other) = delete;
        callback& operator=(callback&& other) noexcept = delete;

        glfw_fun_ptr* add(glfw_fun_ptr fun)
        {
            return _callbacks.emplace_back(std::make_unique<glfw_fun_ptr>(fun)).get();
        }

        void remove(glfw_fun_ptr* fun)
        {
            if(const auto it = std::find_if(_callbacks.begin(), _callbacks.end(), [fun](const std::unique_ptr<glfw_fun_ptr>& p)
            {
                return p.get() == fun;
            }); it != _callbacks.end())
                _callbacks.erase(it);
        }

        template<typename... As, typename = decltype(std::declval<glfw_fun_ptr>()(std::declval<As>()...))>
        void call_all(As&&... as)
        {
            for (auto&& c : _callbacks)
                (*c.get())(std::forward<As&&>(as)...);
        }

    private:
        std::vector<std::unique_ptr<glfw_fun_ptr>> _callbacks;
    };


    struct callbacks
    {
        callbacks(const callbacks& other) = delete;
        callbacks(callbacks&& other) noexcept = default;
        callbacks& operator=(const callbacks& other) = delete;
        callbacks& operator=(callbacks&& other) noexcept = default;

        callbacks() = default;
        explicit callbacks(GLFWwindow* window) { init(window); }

        void init(GLFWwindow* window)
        {
            glfwSetWindowUserPointer(window, this);
            glfwSetKeyCallback(window, [](GLFWwindow* w, int k, int s, int a, int m) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->key_callback.call_all(w, k, s, a, m); });
            glfwSetCharCallback(window, [](GLFWwindow* w, unsigned c) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->char_callback.call_all(w, c); });
            glfwSetCharModsCallback(window, [](GLFWwindow* w, unsigned c, int m) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->char_mods_callback.call_all(w, c, m); });
            glfwSetCursorEnterCallback(window, [](GLFWwindow* w, int e) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->cursor_enter_callback.call_all(w, e); });
            glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->cursor_pos_callback.call_all(w, x, y); });
            glfwSetDropCallback(window, [](GLFWwindow* w, int c, const char** f) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->drop_callback.call_all(w, c, f); });
            glfwSetFramebufferSizeCallback(window, [](GLFWwindow* w, int x, int y) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->framebuffer_size_callback.call_all(w, x, y); });
            glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int k, int a, int m) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->mouse_button_callback.call_all(w, k, a, m); });
            glfwSetScrollCallback(window, [](GLFWwindow* w, double x, double y) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->scroll_callback.call_all(w, x, y); });
            glfwSetWindowCloseCallback(window, [](GLFWwindow* w) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->window_close_callback.call_all(w); });
            glfwSetWindowContentScaleCallback(window, [](GLFWwindow* w, float x, float y) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->window_content_scale_callback.call_all(w, x, y); });
            glfwSetWindowFocusCallback(window, [](GLFWwindow* w, int f) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->window_focus_callback.call_all(w, f); });
            glfwSetWindowIconifyCallback(window, [](GLFWwindow* w, int i) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->window_iconity_callback.call_all(w, i); });
            glfwSetWindowMaximizeCallback(window, [](GLFWwindow* w, int m) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->window_maximize_callback.call_all(w, m); });
            glfwSetWindowPosCallback(window, [](GLFWwindow* w, int x, int y) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->window_pos_callback.call_all(w, x, y); });
            glfwSetWindowRefreshCallback(window, [](GLFWwindow* w) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->window_refresh_callback.call_all(w); });
            glfwSetWindowSizeCallback(window, [](GLFWwindow* w, int x, int y) { static_cast<callbacks*>(glfwGetWindowUserPointer(w))->window_size_callback.call_all(w, x, y); });
        }

        callback<GLFWkeyfun> key_callback;
        callback<GLFWcharfun> char_callback;
        callback<GLFWcharmodsfun> char_mods_callback;
        callback<GLFWcursorenterfun> cursor_enter_callback;
        callback<GLFWcursorposfun> cursor_pos_callback;
        callback<GLFWdropfun> drop_callback;
        callback<GLFWframebuffersizefun> framebuffer_size_callback;
        callback<GLFWmousebuttonfun> mouse_button_callback;
        callback<GLFWscrollfun> scroll_callback;
        callback<GLFWwindowclosefun> window_close_callback;
        callback<GLFWwindowcontentscalefun> window_content_scale_callback;
        callback<GLFWwindowfocusfun> window_focus_callback;
        callback<GLFWwindowiconifyfun> window_iconity_callback;
        callback<GLFWwindowmaximizefun> window_maximize_callback;
        callback<GLFWwindowposfun> window_pos_callback;
        callback<GLFWwindowrefreshfun> window_refresh_callback;
        callback<GLFWwindowsizefun> window_size_callback;
    };
}
