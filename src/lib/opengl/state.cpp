#include "state.hpp"
#include "debug.hpp"

namespace gl
{
    state::state()
    {
        glCreateStatesNV(1, &_id);
    }

    state::~state()
    {
        glDeleteStatesNV(1, &_id);
    }

    void state::capture(basic_primitive primitive) const
    {
        set_debug_callback_enabled(gl::debug_type::other, false);
        glStateCaptureNV(_id, static_cast<uint32_t>(primitive));
        set_debug_callback_enabled(gl::debug_type::other, true);
    }
}
