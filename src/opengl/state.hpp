#pragma once

#include "pipeline.hpp"

namespace gl
{
    class state
    {
    public:
        state() noexcept;
        state(const state&) = delete;
        state(state&& other) noexcept;
        state& operator=(const state&) = delete;
        state& operator=(state&& other) noexcept;
        ~state() noexcept;

        void capture(GLenum primitive) const noexcept;

        operator mygl::state() const noexcept { return _id; }

    private:
        mygl::state _id;
    };
}
