#pragma once

#include <gfx/graphics/device_buffer.hpp>
#include "../general/handle.hpp"
#include <mygl/mygl.hpp>

namespace gfx {
inline namespace v1 {
namespace opengl {
class device_buffer_implementation : public detail::device_buffer_implementation
{
public:
    constexpr static auto flags = GL_DYNAMIC_STORAGE_BIT;

    device_buffer_implementation();
    ~device_buffer_implementation();

    void update_flags(buffer_usage_flags usage) override;
    void allocate(size_type size) override;
    void copy(const std::any& source, const std::any& target, difference_type src_offset, difference_type dst_offset,
              size_type size, fence* f) override;
    void update(difference_type offset, size_type size, const std::byte* data) override;

    handle     api_handle() override;
    mygl::buffer handle() const noexcept;

private:
    movable_handle<mygl::buffer>       _handle;
    buffer_usage_flags _usage;
};
}    // namespace opengl
}    // namespace v1
}    // namespace gfx