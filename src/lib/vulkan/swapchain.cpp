#include <algorithm>
#include "swapchain.hpp"
#include "GLFW/glfw3.h"
#include "device.hpp"

namespace vkn
{
    swapchain::swapchain(device* device, vk::SurfaceKHR surface, uint32_t image_count, bool clipped,
        vk::CompositeAlphaFlagBitsKHR composite_alpha)
        : _device(device), _surface(surface), _image_count(image_count), _clipped(clipped), _composite_alpha(composite_alpha)
    {
        _device->inc_ref();
        const auto capabilities = _device->gpu().getSurfaceCapabilitiesKHR(_surface);

        _swap_semaphore = _device->createSemaphore({});
        _extent = get_extent(capabilities);
        _surface_format = get_format();
        _present_mode = get_present_mode();

        vk::SwapchainCreateInfoKHR swapchain_info;
        swapchain_info.setSurface(_surface)
            .setClipped(_clipped)
            .setCompositeAlpha(_composite_alpha)
            .setPresentMode(_present_mode)
            .setImageExtent(_extent)
            .setImageFormat(_surface_format.format)
            .setImageColorSpace(_surface_format.colorSpace)
            .setMinImageCount(std::clamp(_image_count, capabilities.minImageCount, capabilities.maxImageCount))
            .setImageSharingMode(vk::SharingMode::eExclusive)
            .setImageArrayLayers(1)
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
            .setPreTransform(capabilities.currentTransform)
            .setPQueueFamilyIndices(_device->families().data())
            .setQueueFamilyIndexCount(static_cast<uint32_t>(_device->families().size()));

        _swapchain = _device->createSwapchainKHR(swapchain_info);

        for (auto&& image : _images = _device->getSwapchainImagesKHR(_swapchain))
        {
            vk::ImageViewCreateInfo image_view_info;
            image_view_info.setImage(image)
                .setFormat(_surface_format.format)
                .setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1))
                .setViewType(vk::ImageViewType::e2D);
            _image_views.push_back(jpu::make_ref<texture_view>(_device, image_view_info));
        }
    }

    swapchain::~swapchain()
    {
        _device->destroySemaphore(_swap_semaphore);
        _device->destroySwapchainKHR(_swapchain);
        _device->dec_ref();
    }

    swapchain::operator vk::SwapchainKHR() const
    {
        return _swapchain;
    }

    const vk::Extent2D& swapchain::extent() const
    {
        return _extent;
    }

    const vk::SurfaceFormatKHR& swapchain::surface_format() const
    {
        return _surface_format;
    }

    const std::vector<vk::Image>& swapchain::images() const
    {
        return _images;
    }

    const std::vector<jpu::ref_ptr<texture_view>>& swapchain::image_views() const
    {
        return _image_views;
    }

    bool swapchain::visible() const
    {
        return _extent.width * _extent.height != 0;
    }

    vk::Semaphore swapchain::swap_semaphore() const
    {
        return _swap_semaphore;
    }

    vk::Result swapchain::swap(vk::Fence fence, uint64_t timeout)
    {
        const auto result = _device->acquireNextImageKHR(_swapchain, timeout, _swap_semaphore, fence);
        _last_image = result.value;
        return result.result;
    }

    uint32_t swapchain::current_image() const
    {
        return _last_image;
    }

    vk::Extent2D swapchain::get_extent(const vk::SurfaceCapabilitiesKHR& capabilities) const
    {
        return capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() ?
            capabilities.currentExtent :
            vk::Extent2D(std::clamp(static_cast<uint32_t>(capabilities.currentExtent.width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
                std::clamp(static_cast<uint32_t>(capabilities.currentExtent.height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height));
    }

    vk::SurfaceFormatKHR swapchain::get_format() const
    {
        const auto mk_fmt = [](auto f, auto space) {
            vk::SurfaceFormatKHR fmt;
            fmt.format = f;
            fmt.colorSpace = space;
            return fmt;
        };

        const auto formats = _device->gpu().getSurfaceFormatsKHR(_surface);

        if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined)
            return mk_fmt(vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear);

        for (const auto& surface_format : formats)
            if (surface_format.format == vk::Format::eB8G8R8A8Unorm && surface_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                return surface_format;

        return formats[0];
    }

    vk::PresentModeKHR swapchain::get_present_mode() const
    {
        const auto modes = _device->gpu().getSurfacePresentModesKHR(_surface);
        auto best = vk::PresentModeKHR::eFifo;

        for (const auto& present_mode : modes)
        {
            if (present_mode == vk::PresentModeKHR::eMailbox)
                return present_mode;
            if (present_mode == vk::PresentModeKHR::eImmediate)
                best = present_mode;
        }

        return best;
    }
}
