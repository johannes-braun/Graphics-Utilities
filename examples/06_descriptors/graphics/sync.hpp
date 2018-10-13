#pragma once

#include <vulkan/vulkan.hpp>

namespace gfx
{
	inline namespace v1
	{
		class device;

		class semaphore
		{
		public:
		    explicit semaphore(device& dev);
		    const vk::Semaphore& sem() const noexcept;

		private:
			vk::UniqueSemaphore _sem;
		};

		class fence
		{
		public:
		    explicit fence(device& dev, bool signaled = false);
		    const vk::Fence& fen() const noexcept;

		private:
			vk::UniqueFence _fence;
		};
	}
}