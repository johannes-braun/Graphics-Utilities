#include "commands.hpp"

namespace gfx {
inline namespace v1 {
vk::CommandBuffer const& commands::get_command_buffer() const noexcept
{
    return _buf.get();
}
}    // namespace v1
}    // namespace gfx