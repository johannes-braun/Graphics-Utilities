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
        glStateCaptureNV(_id, static_cast<GLenum>(primitive));
    }
}
