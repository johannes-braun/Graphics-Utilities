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

        operator gl_state_nv_t() const noexcept { return _id; }

    private:
        gl_state_nv_t _id;
    };
}
