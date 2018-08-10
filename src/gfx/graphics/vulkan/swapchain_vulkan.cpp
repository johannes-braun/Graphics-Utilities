#include "init_struct.hpp"
#include "swapchain_vulkan.hpp"
#include "image_view_vulkan.hpp"
#include "result.hpp"

namespace gfx {
inline namespace v1 {
namespace vulkan {

uint32_t swapchain_implementation::current_image() const noexcept
{
    return _current_image;
}

void swapchain_implementation::present()
{
    if (_presented) {
        std::array<VkSemaphore, 1>          wait_semaphores{_present_semaphore};
        std::array<VkPipelineStageFlags, 1> wait_masks{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        std::array<VkCommandBuffer, 1>      command_buffers{_primary_command_buffers[_current_image]};
        init<VkSubmitInfo>                  submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
        submit.pWaitSemaphores      = std::data(wait_semaphores);
        submit.waitSemaphoreCount   = std::size(wait_semaphores);
        submit.pSignalSemaphores    = &_render_semaphore;
        submit.signalSemaphoreCount = 1;
        submit.pWaitDstStageMask    = std::data(wait_masks);
        // In graphics queue. Waits on all posted semaphores.
		check_result(vkQueueSubmit(_graphics_queue, 1, &submit, _render_fences[_current_image]));

        uint32_t               idx = _current_image;
        init<VkPresentInfoKHR> present_info{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
        present_info.pImageIndices      = &idx;
        present_info.pSwapchains        = &_swapchain;
        present_info.swapchainCount     = 1;
        present_info.pWaitSemaphores    = &_render_semaphore;
        present_info.waitSemaphoreCount = 1;
		VkResult swapchain_result;
		present_info.pResults = &swapchain_result;
        // Solely in present queue, but waits for _render_semaphore which is triggered only after all posted semaphores are signaled.
        VkResult present_result = check_result(vkQueuePresentKHR(_present_queue, &present_info));
		check_result(swapchain_result);
    }
	check_result(vkAcquireNextImageKHR(_device, _swapchain, std::numeric_limits<uint64_t>::max(), _present_semaphore, nullptr, &_current_image));
    // Wait until last frame using this image has finished rendering
	check_result(vkWaitForFences(_device, 1, &_render_fences[_current_image], true, std::numeric_limits<uint64_t>::max()));
	check_result(vkResetFences(_device, 1, &_render_fences[_current_image]));

    _presented = true;
}

void swapchain_implementation::resize(uint32_t width, uint32_t height)
{
    auto& ctx = context::current();
    _ctx_impl = static_cast<context_implementation*>(std::any_cast<detail::context_implementation*>(ctx->implementation()));

	if (!_render_fences.empty())
		vkWaitForFences(_ctx_impl->device(), _render_fences.size(), _render_fences.data(), true, std::numeric_limits<uint64_t>::max());

	vkDeviceWaitIdle(_ctx_impl->device());
	this->~swapchain_implementation();
	_presented = false;
    _device = _ctx_impl->device();
    init<VkSwapchainCreateInfoKHR> swapchain_info{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapchain_info.clipped               = true;
    swapchain_info.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_info.imageArrayLayers      = 1;
    swapchain_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_info.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    swapchain_info.pQueueFamilyIndices   = &(_ctx_impl->queue_families()[fam::present]);
    swapchain_info.queueFamilyIndexCount = 1;

    VkSurfaceCapabilitiesKHR capabilities;
	check_result(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_ctx_impl->gpu(), _ctx_impl->surface(), &capabilities));
    swapchain_info.surface       = _ctx_impl->surface();
    swapchain_info.imageExtent   = VkExtent2D{width, height};
    swapchain_info.minImageCount = ctx->options().framebuffer_images;
    swapchain_info.preTransform  = capabilities.currentTransform;

    u32 fmt_count = 0;
	check_result(vkGetPhysicalDeviceSurfaceFormatsKHR(_ctx_impl->gpu(), _ctx_impl->surface(), &fmt_count, nullptr));
    std::vector<VkSurfaceFormatKHR> formats(fmt_count);
	check_result(vkGetPhysicalDeviceSurfaceFormatsKHR(_ctx_impl->gpu(), _ctx_impl->surface(), &fmt_count, formats.data()));
    if (const auto it = std::find_if(formats.begin(), formats.end(),
                                     [](const VkSurfaceFormatKHR& fmt) {
                                         return fmt.format == VK_FORMAT_B8G8R8A8_UNORM
                                                && fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
                                     });
        it == formats.end())
    {
        elog << "Did not find bgra8 format with srgb-nonlinear color space.";
    }
    else
    {
        swapchain_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        swapchain_info.imageFormat     = VK_FORMAT_B8G8R8A8_UNORM;
    }

    u32 pm_count = 0;
	check_result(vkGetPhysicalDeviceSurfacePresentModesKHR(_ctx_impl->gpu(), _ctx_impl->surface(), &pm_count, nullptr));
    std::vector<VkPresentModeKHR> present_modes(pm_count);
	check_result(vkGetPhysicalDeviceSurfacePresentModesKHR(_ctx_impl->gpu(), _ctx_impl->surface(), &pm_count, present_modes.data()));
    if (const auto it = std::find_if(present_modes.begin(), present_modes.end(),
                                     [](const VkPresentModeKHR& mode) { return mode == VK_PRESENT_MODE_MAILBOX_KHR; });
        it == present_modes.end())
    {
        elog << "Did not find mailbox present mode.";
    }
    else
        swapchain_info.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;

	check_result(vkCreateSwapchainKHR(_device, &swapchain_info, nullptr, &_swapchain));

    _present_queue  = _ctx_impl->queues()[fam::present];
    _graphics_queue = _ctx_impl->queues()[fam::graphics];

    // TODO:
    u32 img_count = 0;
	check_result(vkGetSwapchainImagesKHR(_device, _swapchain, &img_count, nullptr));
    _temp_images.resize(img_count);
	check_result(vkGetSwapchainImagesKHR(_device, _swapchain, &img_count, _temp_images.data()));

    init<VkCommandBufferAllocateInfo> cmd_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    cmd_info.commandBufferCount = static_cast<uint32_t>(_temp_images.size());
    cmd_info.commandPool        = _ctx_impl->command_pools()[fam::graphics];
    cmd_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    _primary_command_buffers.resize(_temp_images.size());
	check_result(vkAllocateCommandBuffers(_device, &cmd_info, _primary_command_buffers.data()));

    _command_pool = _ctx_impl->command_pools()[fam::graphics];

    init<VkSemaphoreCreateInfo> sem_info{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
	check_result(vkCreateSemaphore(_device, &sem_info, nullptr, &_present_semaphore));
	check_result(vkCreateSemaphore(_device, &sem_info, nullptr, &_render_semaphore));

    init<VkFenceCreateInfo> fen_info{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fen_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	for (int i = 0; i < _temp_images.size(); ++i)
	{
		check_result(vkCreateFence(_device, &fen_info, nullptr, &_render_fences.emplace_back()));

		image_view& view = _image_views.emplace_back();
		static_cast<image_view_implementation*>(&*view.implementation())->initialize_vk(gfx::imgv_type::image2d, gfx::format::bgra8unorm, _temp_images[i], 0, 1, 0, 1);
	}
}

const std::vector<image_view>& swapchain_implementation::image_views() const
{
    return _image_views;
}

handle swapchain_implementation::api_handle()
{
    return _swapchain;
}

swapchain_implementation::~swapchain_implementation()
{
    if (_swapchain) vkDestroySwapchainKHR(_device, _swapchain, nullptr);

    for (auto& f : _render_fences) 
		vkDestroyFence(_device, f, nullptr);

    if (_device && !_primary_command_buffers.empty())
        vkFreeCommandBuffers(_device, _command_pool, _primary_command_buffers.size(), _primary_command_buffers.data());

    if (_present_semaphore) vkDestroySemaphore(_device, _present_semaphore, nullptr);
    if (_render_semaphore) vkDestroySemaphore(_device, _render_semaphore, nullptr);
}
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx