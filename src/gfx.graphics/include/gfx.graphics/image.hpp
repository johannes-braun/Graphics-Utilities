#pragma once

#include <vulkan/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

namespace gfx {
inline namespace v1 {
class device;
class image
{
public:
    image(device& dev, const vk::ImageCreateInfo& create_info);
    ~image();
    image(const image& other) = delete;
	image(image&& other) noexcept;
    image& operator=(const image& other) = delete;
    image& operator=(image&& other) noexcept;

    const vk::Image& get_image() const noexcept { return _image; }

private:
    device*       _device;
    vk::Image     _image;
    VmaAllocation _alloc;
};
}    // namespace v1
}    // namespace gfx