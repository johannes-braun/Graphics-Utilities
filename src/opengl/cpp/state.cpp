#include "../state.hpp"

namespace gl
{
    state::state() noexcept
    {
        glCreateStatesNV(1, &_id);
    }

    state::state(state&& other) noexcept
    {
        operator=(std::forward<state&&>(other));
    }

    state& state::operator=(state&& other) noexcept
    {
        _id = other._id;
        other._id = mygl::state::zero;
        return *this;
    }

    state::~state() noexcept
    {
        if(_id != mygl::state::zero)
            glDeleteStatesNV(1, &_id);
    }

    void state::capture(GLenum primitive) const noexcept
    {
        glStateCaptureNV(_id, static_cast<GLenum>(primitive));
    }
}
