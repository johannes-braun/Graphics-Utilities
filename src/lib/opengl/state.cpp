#include "state.hpp"
#include "debug.hpp"

namespace gl
{
    state::state() noexcept
    {
        glCreateStatesNV(1, &_id);
    }

    state::~state() noexcept
    {
        glDeleteStatesNV(1, &_id);
    }

    void state::capture(basic_primitive primitive) const noexcept
    {
        set_debug_callback_enabled(gl::debug_type::other, false);
        glStateCaptureNV(_id, static_cast<GLenum>(primitive));
        set_debug_callback_enabled(gl::debug_type::other, true);
    }
}
