#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>
#include "create_info.hpp"
#include "texture.hpp"
#include <jpu/memory>

namespace vkn
{
    class device;

    class swapchain : public jpu::ref_count
    {
    public:
        swapchain(device* device, vk::SurfaceKHR surface, uint32_t image_count = 8, bool clipped = true, vk::CompositeAlphaFlagBitsKHR composite_alpha = vk::CompositeAlphaFlagBitsKHR::eOpaque);
        ~swapchain();

        operator vk::SwapchainKHR() const;

        const vk::Extent2D& extent() const;
        const vk::SurfaceFormatKHR& surface_format() const;
        const std::vector<vk::Image>& images() const;
        const std::vector<jpu::ref_ptr<TextureView>>& image_views() const;
        bool visible() const;

        vk::Semaphore swap_semaphore() const;
        vk::Result swap(vk::Fence fence = nullptr, uint64_t timeout = std::numeric_limits<uint64_t>::max());
        uint32_t current_image() const;

    private:
        vk::Extent2D get_extent(const vk::SurfaceCapabilitiesKHR& capabilities) const;
        vk::SurfaceFormatKHR get_format() const;
        vk::PresentModeKHR get_present_mode() const;

        device* _device;
        vk::SurfaceKHR _surface;
        uint32_t _image_count;
        bool _clipped;
        vk::CompositeAlphaFlagBitsKHR _composite_alpha;

        uint32_t _last_image = 0;
        vk::SwapchainKHR _swapchain;
        vk::Extent2D _extent;
        vk::SurfaceFormatKHR _surface_format;
        vk::PresentModeKHR _present_mode;
        std::vector<vk::Image> _images;
        std::vector<jpu::ref_ptr<TextureView>> _image_views;
        vk::Semaphore _swap_semaphore;
    };
}
