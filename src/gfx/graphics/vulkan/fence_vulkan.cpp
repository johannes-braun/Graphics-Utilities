#include "fence_vulkan.hpp"
#include "init_struct.hpp"

namespace gfx {
inline namespace v1 {
namespace vulkan {


fence_implementation::~fence_implementation()
{
    if (_fence) vkDestroyFence(_device, _fence, nullptr);
}

void fence_implementation::wait(std::chrono::nanoseconds timeout)
{
	vkWaitForFences(_device, 1, &_fence, true, timeout.count());
}

std::any fence_implementation::api_handle()
{
    if(!_fence)
    {
		init<VkFenceCreateInfo> info{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
		vkCreateFence(_device, &info, nullptr, &_fence);
    }
	return VkFence(_fence);
}
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx