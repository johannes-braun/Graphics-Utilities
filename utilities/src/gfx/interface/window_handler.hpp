#pragma once

namespace gfx
{
    class window;
    struct window_handler
    {
        friend window;
        window_handler(window& w);
        virtual ~window_handler() = default;

    protected:
        window& get_window() const noexcept;
        virtual void do_update() = 0;

    private:
        window& _window;
    };

    struct window_handler_opengl : window_handler
    {
        window_handler_opengl(window& w);
    protected:
        void do_update() override;
    };

    struct window_handler_vulkan : window_handler
    {
        window_handler_vulkan(window& w);
    protected:
        void do_update() override;
    };
}
