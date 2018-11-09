#include "commands.hpp"

namespace gfx
{
	inline namespace v1
	{
		const vk::CommandBuffer& commands::cmd() const noexcept { return _buf.get(); }
	}
}