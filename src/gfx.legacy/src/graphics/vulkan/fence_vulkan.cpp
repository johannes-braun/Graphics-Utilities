#include "fence_vulkan.hpp"
#include "init_struct.hpp"
#include "context_vulkan.hpp"
#include "result.hpp"

namespace gfx {
inline namespace v1 {
namespace vulkan {


fence_implementation::~fence_implementation()
{
    if (_fence) vkDestroyFence(_device, _fence, nullptr);
}

void fence_implementation::wait(std::chrono::nanoseconds timeout)
{
	check_result(vkWaitForFences(_device, 1, &_fence, true, timeout.count()));
	check_result(vkResetFences(_device, 1, &_fence));
}

std::any fence_implementation::api_handle()
{
    if(!_fence)
    {
		auto&                    ctx = context::current();
		auto impl = static_cast<context_implementation*>(std::any_cast<detail::context_implementation*>(ctx->implementation()));
		_device = impl->device();

		init<VkFenceCreateInfo> info{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
		check_result(vkCreateFence(_device, &info, nullptr, &_fence));
    }
	return VkFence(_fence);
}
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx