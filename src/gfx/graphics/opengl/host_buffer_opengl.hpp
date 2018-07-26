#pragma once
#include <gfx/graphics/host_buffer.hpp>
#include "../general/handle.hpp"
#include <mygl/mygl.hpp>

namespace gfx {
inline namespace v1 {
namespace opengl {
class host_buffer_implementation : public detail::host_buffer_implementation
{
public:
    constexpr static auto flags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT;
    constexpr static auto map_flags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT;

    host_buffer_implementation();
    ~host_buffer_implementation();

    allocation allocate(size_type size) override;
    void       deallocate(const allocation& alloc) override;

	handle api_handle() override;

private:
    movable_handle<mygl::buffer> _handle;
};
}    // namespace opengl
}    // namespace v1
}    // namespace gfx
