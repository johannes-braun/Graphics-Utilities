#include "vulkan/commands.hpp"

namespace gfx {
inline namespace v1 {
namespace vulkan {
vk::CommandBuffer const& commands::get_command_buffer() const noexcept
{
    return _buf.get();
}
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx