#pragma once
#include <gfx/graphics/host_buffer.hpp>
#include <mygl/mygl.hpp>

namespace gfx::opengl
{
class host_buffer_implementation : public detail::host_buffer_implementation
{
public:
    constexpr static auto flags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT;
    constexpr static auto map_flags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT;

    host_buffer_implementation();
    ~host_buffer_implementation();
    std::byte*   grow(const std::byte* old_data, size_type old_size, size_type new_capacity) override;
    mygl::buffer handle() const noexcept;

private:
    mygl::buffer _handle;
};
} // namespace gfx
