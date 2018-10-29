#pragma once

#include <graphics/fence.hpp>
#include "../general/handle.hpp"
#include <mygl/mygl.hpp>

namespace gfx {
inline namespace v1 {
namespace opengl {

inline void push_fence(fence* f)
{
    if (f) {
        GLsync& sync = *std::any_cast<GLsync*>(f->api_handle());
        if (glIsSync(sync)) glDeleteSync(sync);
        sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, GLbitfield(0));
    }
}

class fence_implementation : public detail::fence_implementation
{
public:
    fence_implementation() = default;
    ~fence_implementation()
    {
        if (glIsSync(_sync)) glDeleteSync(_sync);
    }
    void wait(std::chrono::nanoseconds timeout) override
    {
		if (glIsSync(_sync))
		{
			glClientWaitSync(_sync, GLbitfield(0), timeout.count());
			glDeleteSync(_sync);
		}
    }
	handle api_handle() override { return &(_sync.get()); }

    void push() { _sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, GLbitfield(0)); }

private:
    movable_handle<GLsync> _sync = nullptr;
};
}    // namespace opengl
}    // namespace v1
}    // namespace gfx
