#pragma once

#include <memory>
#include <window/window.hpp>

namespace gfx
{
    class host
    {
    public:

    private:
        std::unique_ptr<wf::window> _window;
    };
}