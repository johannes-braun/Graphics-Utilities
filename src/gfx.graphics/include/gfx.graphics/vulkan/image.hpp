#pragma once

#include "allocator.hpp"
#include <vulkan/vulkan.hpp>

namespace gfx {
inline namespace v1 {
namespace vulkan {
class device;
class image
{
public:
    friend class swapchain;

    image(device& dev, vk::ImageCreateInfo const& create_info);
    ~image();
    image(image const& other) = delete;
    image(image&& other) noexcept;
    image& operator=(image const& other) = delete;
    image& operator                      =(image&& other) noexcept;

    [[nodiscard]] auto get_image() const noexcept -> vk::Image const& { return _image; }

private:
    image() = default;

    device*       _device = nullptr;
    vk::Image     _image;
    VmaAllocation _alloc = nullptr;
};
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx