#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>
#include "create_info.hpp"
#include "texture.hpp"
#include <jpu/memory>

namespace vkn
{
    class Window;
    class device;

    struct SwapchainCreateInfo
    {
        SwapchainCreateInfo(
            vk::SurfaceKHR surface = nullptr,
            device* device = nullptr,
            const uint32_t min_image_count = 1,
            const bool clipped = true,
            const vk::CompositeAlphaFlagBitsKHR composite_alpha = vk::CompositeAlphaFlagBitsKHR::eOpaque)
            : surface(surface), device(device), min_image_count(min_image_count), clipped(clipped), composite_alpha(composite_alpha) {}
        
        SwapchainCreateInfo& setDevice(device* value) { device = value; return *this; }
        SwapchainCreateInfo& setSurface(const vk::SurfaceKHR value) { surface = value; return *this; }
        SwapchainCreateInfo& setMinImageCount(const uint32_t value) { min_image_count = value; return *this; }
        SwapchainCreateInfo& setClipped(const bool value) { clipped = value; return *this; }
        SwapchainCreateInfo& setCompositeAlpha(const vk::CompositeAlphaFlagBitsKHR value) { composite_alpha = value; return *this; }

        device* device;
        vk::SurfaceKHR surface;
        uint32_t min_image_count;
        bool clipped;
        vk::CompositeAlphaFlagBitsKHR composite_alpha;
    };

    class Swapchain : ClassInfo<SwapchainCreateInfo, Swapchain>, public jpu::ref_count
    {
    public:
        explicit Swapchain(const SwapchainCreateInfo& info);
        ~Swapchain();

        vk::SwapchainKHR swapchain() const;
        operator vk::SwapchainKHR() const;

        const vk::Extent2D& extent() const;
        const vk::SurfaceFormatKHR& surfaceFormat() const;
        bool visible() const;

        const std::vector<vk::Image>& images() const;
        const std::vector<jpu::ref_ptr<TextureView>>& imageViews() const;

        vk::Semaphore imageSemaphore() const;
        vk::Result swap(vk::Fence fence = nullptr, uint64_t timeout = std::numeric_limits<uint64_t>::max());
        uint32_t currentImage() const;

    private:
        vk::Extent2D fetchExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const;
        vk::SurfaceFormatKHR fetchFormat() const;
        vk::PresentModeKHR fetchPresentMode() const;

        uint32_t m_last_image = 0;
        vk::SwapchainKHR m_swapchain;
        vk::Extent2D m_extent;
        vk::SurfaceFormatKHR m_surface_format;
        vk::PresentModeKHR m_present_mode;
        std::vector<vk::Image> m_images;
        std::vector<jpu::ref_ptr<TextureView>> m_image_views;
        vk::Semaphore m_image_semaphore;
    };
}
