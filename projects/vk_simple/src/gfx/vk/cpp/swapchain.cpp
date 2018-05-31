#include "../swapchain.hpp"

#include "../surface.hpp"
#include "../device.hpp"
#include "../semaphore.hpp"
#include "../fence.hpp"
#include "../image.hpp"

namespace vk
{
    swapchain::swapchain(const std::shared_ptr<device>& device, const swapchain_create_info& create_info)
        : _device(device)
    {
        VkSwapchainCreateInfoKHR info;
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.pNext = nullptr;
        info.flags                  = create_info._flags;
        info.clipped                = create_info.clipped;
        info.compositeAlpha         = create_info.composite_alpha;
        info.imageArrayLayers       = create_info.image_array_layers;
        info.imageColorSpace        = create_info.color_space;
        info.imageExtent            = create_info.extent;
        info.imageFormat            = create_info.format;
        info.imageSharingMode       = create_info.image_sharing_mode;
        info.imageUsage             = create_info.image_usage;
        info.minImageCount          = create_info.min_image_count;
        info.oldSwapchain           = create_info.old_swapchain ? *(create_info.old_swapchain) : nullptr;
        info.pQueueFamilyIndices    = create_info.queue_family_indices.data();
        info.queueFamilyIndexCount  = uint32_t(create_info.queue_family_indices.size());
        info.presentMode            = create_info.present_mode;
        info.preTransform           = create_info.pre_transform;
        info.surface                = *(create_info.surface);

        vkCreateSwapchainKHR(*_device, &info, nullptr, &_swapchain);

        uint32_t count;
        vkGetSwapchainImagesKHR(*_device, _swapchain, &count, nullptr);
        std::vector<VkImage> imgs(count);
        vkGetSwapchainImagesKHR(*_device, _swapchain, &count, imgs.data());
        _images.resize(count);
        for (int i=0; i<imgs.size(); ++i)
        {
            _images[i] = std::shared_ptr<image>(new image());
            _images[i]->_owning = false;
            _images[i]->_image = imgs[i];
            _images[i]->_device = _device;
        }
    }

    swapchain::~swapchain()
    {
        vkDestroySwapchainKHR(*_device, _swapchain, nullptr);
    }

    std::vector<std::shared_ptr<image>> swapchain::images() const noexcept
    {
        return _images;
    }

    swapchain::operator VkSwapchainKHR() const noexcept
    {
        return _swapchain;
    }

    std::pair<VkResult, uint32_t> swapchain::next_image(const std::shared_ptr<semaphore>& semaphore, const std::shared_ptr<fence>& fence, uint64_t timeout) const noexcept
    {
        uint32_t idx;
        return std::make_pair(vkAcquireNextImageKHR(*_device, _swapchain, timeout, semaphore ? *semaphore : nullptr, fence ? *fence : nullptr, &idx), idx);
    }
}