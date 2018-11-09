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
			semaphore(const semaphore& other);
			semaphore& operator=(const semaphore& other) noexcept;
			semaphore(semaphore&& other) noexcept = default;
			semaphore& operator=(semaphore&& other) noexcept = default;

		    const vk::Semaphore& sem() const noexcept;

		private:
			vk::UniqueSemaphore _sem;
		};

		class fence
		{
		public:
		    explicit fence(device& dev, bool signaled = false);
			fence(const fence& other);
			fence& operator=(const fence& other) noexcept;
			fence(fence&& other) noexcept = default;
			fence& operator=(fence&& other) noexcept = default;

		    const vk::Fence& fen() const noexcept;

		private:
			vk::UniqueFence _fence;
		};
	}
}