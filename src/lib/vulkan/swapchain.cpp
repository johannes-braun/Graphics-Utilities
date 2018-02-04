#include <algorithm>
#include "swapchain.hpp"
#include "GLFW/glfw3.h"
#include "device.hpp"

namespace vkn
{
    Swapchain::Swapchain(const SwapchainCreateInfo& info)
        : ClassInfo(info)
    {
        m_info.device->inc_ref();

        const auto capabilities = m_info.device->gpu().getSurfaceCapabilitiesKHR(m_info.surface);

        m_image_semaphore = m_info.device->createSemaphore({});
        m_extent = fetchExtent(capabilities);
        m_surface_format = fetchFormat();
        m_present_mode = fetchPresentMode();

        vk::SwapchainCreateInfoKHR swapchain_info;
        swapchain_info.setSurface(m_info.surface)
            .setClipped(m_info.clipped)
            .setCompositeAlpha(m_info.composite_alpha)
            .setPresentMode(m_present_mode)
            .setImageExtent(m_extent)
            .setImageFormat(m_surface_format.format)
            .setImageColorSpace(m_surface_format.colorSpace)
            .setMinImageCount(std::clamp(m_info.min_image_count, capabilities.minImageCount, capabilities.maxImageCount))
            .setImageSharingMode(vk::SharingMode::eExclusive)
            .setImageArrayLayers(1)
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
            .setPreTransform(capabilities.currentTransform)
            .setPQueueFamilyIndices(m_info.device->families().data())
            .setQueueFamilyIndexCount(static_cast<uint32_t>(m_info.device->families().size()));

        m_swapchain = m_info.device->createSwapchainKHR(swapchain_info);

        for (auto&& image : m_images = m_info.device->getSwapchainImagesKHR(m_swapchain))
        {
            vk::ImageViewCreateInfo image_view_info;
            image_view_info.setImage(image)
                .setFormat(m_surface_format.format)
                .setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1))
                .setViewType(vk::ImageViewType::e2D);
            m_image_views.push_back(jpu::make_ref<TextureView>(m_info.device, image_view_info));
        }
    }

    Swapchain::~Swapchain()
    {
        m_info.device->destroySemaphore(m_image_semaphore);
        m_info.device->destroySwapchainKHR(m_swapchain);

        m_info.device->dec_ref();
    }

    vk::SwapchainKHR Swapchain::swapchain() const
    {
        return m_swapchain;
    }

    Swapchain::operator vk::SwapchainKHR() const
    {
        return m_swapchain;
    }

    const vk::Extent2D& Swapchain::extent() const
    {
        return m_extent;
    }

    const vk::SurfaceFormatKHR& Swapchain::surfaceFormat() const
    {
        return m_surface_format;
    }

    bool Swapchain::visible() const
    {
        return m_extent.width * m_extent.height != 0;
    }

    const std::vector<vk::Image>& Swapchain::images() const
    {
        return m_images;
    }

    const std::vector<jpu::ref_ptr<TextureView>>& Swapchain::imageViews() const
    {
        return m_image_views;
    }

    vk::Semaphore Swapchain::imageSemaphore() const
    {
        return m_image_semaphore;
    }

    vk::Result Swapchain::swap(const vk::Fence fence, const uint64_t timeout)
    {
        const auto result = m_info.device->acquireNextImageKHR(m_swapchain, timeout, m_image_semaphore, fence);
        m_last_image = result.value;
        return result.result;
    }

    uint32_t Swapchain::currentImage() const
    {
        return m_last_image;
    }

    vk::Extent2D Swapchain::fetchExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const
    {
        return capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() ?
            capabilities.currentExtent :
            vk::Extent2D(std::clamp(static_cast<uint32_t>(capabilities.currentExtent.width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
                std::clamp(static_cast<uint32_t>(capabilities.currentExtent.height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height));
    }

    vk::SurfaceFormatKHR Swapchain::fetchFormat() const
    {
        const auto mk_fmt = [](auto f, auto space) {
            vk::SurfaceFormatKHR fmt;
            fmt.format = f;
            fmt.colorSpace = space;
            return fmt;
        };

        const auto formats = m_info.device->gpu().getSurfaceFormatsKHR(m_info.surface);

        if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined)
            return mk_fmt(vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear);

        for (const auto& surface_format : formats)
            if (surface_format.format == vk::Format::eB8G8R8A8Unorm && surface_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                return surface_format;

        return formats[0];
    }

    vk::PresentModeKHR Swapchain::fetchPresentMode() const
    {
        const auto modes = m_info.device->gpu().getSurfacePresentModesKHR(m_info.surface);
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
