#pragma once

#include <functional>
#include "rect.hpp"
#include <io/window.hpp>

namespace gfx::ui
{
    class window;
    struct rect;

    class layout
    {
    public:
        friend window;

        void fill(std::function<void(layout& layout, window& parent)> fill_func);
        virtual rect get_rect() const noexcept;
        rect get_clip_rect() const noexcept;
        virtual ~layout() = default;

        virtual void force_layout(const rect& new_area) = 0;

    protected:
        virtual void update() = 0;
        layout(window* parent, const rect& area);
        layout(const layout&) = delete;
        layout(layout&&) = default;

        rect _rect;
        window* _parent = nullptr;
    };

    class linear_layout : public layout
    {
    public:
        friend window;

        virtual rect get_rect() const noexcept override;
        void add_content_height(float h) noexcept;
        void force_layout(const rect& new_area) override;

    private:
        linear_layout(window* parent, const rect& area);
        virtual void update() override;

        rect _layout_rect;
    };

    class scroll_layout : public layout
    {
    public:
        friend window;
        ~scroll_layout();
        virtual rect get_rect() const noexcept override;
        void add_content_height(float h) noexcept;
        void force_layout(const rect& new_area) override;

    protected:
        std::function<void(GLFWwindow*, double, double)>* _callback;
        scroll_layout(window* parent, const rect& area);
        virtual void update() override;
        float _scroll = 0.f;
        float _smooth_scroll = 0.f;
        rect _last_scroll_rect;
        rect _scroll_rect;
    };
}