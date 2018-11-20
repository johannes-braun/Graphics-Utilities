#include "commands.hpp"

namespace gfx {
inline namespace v1 {
vk::CommandBuffer const& commands::cmd() const noexcept
{
    return _buf.get();
}
}    // namespace v1
}    // namespace gfx