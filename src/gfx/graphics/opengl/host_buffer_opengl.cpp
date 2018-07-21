#include "host_buffer_opengl.hpp"
#include <cstring>

namespace gfx {
inline namespace v1 {
namespace opengl {
host_buffer_implementation::host_buffer_implementation() {}

host_buffer_implementation::~host_buffer_implementation()
{
    glFinish();
}

host_buffer_implementation::allocation host_buffer_implementation::allocate(size_type size)
{
    mygl::buffer hnd;
    glCreateBuffers(1, &hnd);
    glNamedBufferStorage(hnd, size, nullptr, flags);
    allocation alloc;
    alloc.data   = glMapNamedBufferRange(hnd, 0, size, map_flags);
    alloc.handle = hnd;
    _handle      = hnd;
    return alloc;
}

void host_buffer_implementation::deallocate(const allocation& alloc)
{
    if (alloc.handle.has_value()) {
        auto hnd = std::any_cast<mygl::buffer>(alloc.handle);
        if (glIsBuffer(hnd)) glDeleteBuffers(1, &hnd);
    }
}

std::any host_buffer_implementation::api_handle()
{
    return _handle;
}
}    // namespace opengl
}    // namespace v1
}    // namespace gfx