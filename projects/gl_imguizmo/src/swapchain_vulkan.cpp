#include "swapchain_vulkan.hpp"
#include "context_vulkan.hpp"
#include <gfx/log.hpp>

namespace gfx::vulkan
{
vk::UniqueSwapchainKHR create_swapchain(const vk::PhysicalDevice& gpu, const vk::Device& device, const vk::SurfaceKHR& surface,
                                        uint32_t family, const vk::Extent2D& extent)
{
    vk::SwapchainCreateInfoKHR swapchain_info;
    swapchain_info.clipped               = true;
    swapchain_info.compositeAlpha        = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapchain_info.imageArrayLayers      = 1;
    swapchain_info.imageSharingMode      = vk::SharingMode::eExclusive;
    swapchain_info.imageUsage            = vk::ImageUsageFlagBits::eColorAttachment;
    swapchain_info.pQueueFamilyIndices   = &family;
    swapchain_info.queueFamilyIndexCount = 1;

    const vk::SurfaceCapabilitiesKHR capabilities = gpu.getSurfaceCapabilitiesKHR(surface);
    swapchain_info.surface                        = surface;
    swapchain_info.imageExtent                    = extent;
    swapchain_info.minImageCount                  = capabilities.minImageCount;
    swapchain_info.preTransform                   = capabilities.currentTransform;

    const std::vector<vk::SurfaceFormatKHR> formats = gpu.getSurfaceFormatsKHR(surface);
    if(const auto it = std::find_if(formats.begin(),
                                    formats.end(),
                                    [](const vk::SurfaceFormatKHR& fmt) {
                                        return fmt.format == vk::Format::eB8G8R8A8Unorm &&
                                               fmt.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
                                    });
       it == formats.end())
    {
        gfx::cloge << "Did not find bgra8 format with srgb-nonlinear color space.";
    }
    else
    {
        swapchain_info.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
        swapchain_info.imageFormat     = vk::Format::eB8G8R8A8Unorm;
    }
    const std::vector<vk::PresentModeKHR> present_modes = gpu.getSurfacePresentModesKHR(surface);
    if(const auto it = std::find_if(present_modes.begin(),
                                    present_modes.end(),
                                    [](const vk::PresentModeKHR& mode) { return mode == vk::PresentModeKHR::eMailbox; });
       it == present_modes.end())
    {
        gfx::cloge << "Did not find mailbox present mode.";
    }
    else
        swapchain_info.presentMode = vk::PresentModeKHR::eMailbox;
    return device.createSwapchainKHRUnique(swapchain_info);
}

swapchain_implementation::swapchain_implementation()
{
    auto&      ctx  = context::current();
    const auto impl = static_cast<context_implementation*>(std::any_cast<detail::context_implementation*>(ctx->implementation()));

    int w, h;
    glfwGetFramebufferSize(ctx->window(), &w, &h);
    const vk::Extent2D swapchain_size(w, h);

    _device         = impl->device();
    _swapchain      = create_swapchain(impl->gpu(), impl->device(), impl->surface(), impl->families()[fam::present], swapchain_size);
    _images         = impl->device().getSwapchainImagesKHR(_swapchain.get());
    _present_queue  = impl->queues()[fam::present];
    _graphics_queue = impl->queues()[fam::graphics];

    vk::CommandBufferAllocateInfo info;
    info.commandBufferCount  = static_cast<uint32_t>(_images.size());
    info.commandPool         = impl->command_pools()[fam::graphics].get();
    info.level               = vk::CommandBufferLevel::ePrimary;
    _primary_command_buffers = impl->device().allocateCommandBuffersUnique(info);
    _present_semaphore       = impl->device().createSemaphoreUnique({});
    _render_semaphore        = impl->device().createSemaphoreUnique({});

    for(int i = 0; i < _images.size(); ++i)
        _render_fences.push_back(_device.createFenceUnique({vk::FenceCreateFlagBits::eSignaled}));
}

uint32_t swapchain_implementation::current_image() const noexcept { return _current_image; }

void swapchain_implementation::present()
{
    if(_current_image != -1)
    {
        // TODO do better:
        _primary_command_buffers[_current_image]->reset({});
        _primary_command_buffers[_current_image]->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
        _primary_command_buffers[_current_image]->end();

        std::array<vk::Semaphore, 1>          wait_semaphores{_present_semaphore.get()};
        std::array<vk::PipelineStageFlags, 1> wait_masks{vk::PipelineStageFlagBits::eColorAttachmentOutput};
        std::array<vk::CommandBuffer, 1>      command_buffers{_primary_command_buffers[_current_image].get()};
        vk::SubmitInfo                        submit;
        submit.commandBufferCount   = std::size(command_buffers);
        submit.pCommandBuffers      = std::data(command_buffers);
        submit.pWaitSemaphores      = std::data(wait_semaphores);
        submit.waitSemaphoreCount   = std::size(wait_semaphores);
        submit.pSignalSemaphores    = &_render_semaphore.get();
        submit.signalSemaphoreCount = 1;
        submit.pWaitDstStageMask    = std::data(wait_masks);
        _graphics_queue.submit(submit, _render_fences[_current_image].get());

        uint32_t           idx = _current_image;
        vk::PresentInfoKHR present_info;
        present_info.pImageIndices      = &idx;
        present_info.pSwapchains        = &_swapchain.get();
        present_info.swapchainCount     = 1;
        present_info.pWaitSemaphores    = &_render_semaphore.get();
        present_info.waitSemaphoreCount = 1;
        vk::Result present_result       = _present_queue.presentKHR(present_info);
    }

    _current_image = _device.acquireNextImageKHR(*_swapchain, std::numeric_limits<uint64_t>::max(), *_present_semaphore, nullptr).value;
    // Wait until last frame using this image has finished rendering
    _device.waitForFences(*_render_fences[_current_image], true, std::numeric_limits<uint64_t>::max());
    _device.resetFences(*_render_fences[_current_image]);
}
} // namespace gfx::vulkan
