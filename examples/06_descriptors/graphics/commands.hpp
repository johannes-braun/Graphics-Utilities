#pragma once

#include <vulkan/vulkan.hpp>

namespace gfx
{
	inline namespace v1
	{
		class commands
		{
		public:
			const vk::CommandBuffer& cmd() const noexcept;

		private:
			vk::UniqueCommandBuffer _buf;
		};
	}
}