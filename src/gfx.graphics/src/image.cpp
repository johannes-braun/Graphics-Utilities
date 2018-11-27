#include "image.hpp"
#include "device.hpp"

namespace gfx {
inline namespace v1 {
image::image(device& dev, vk::ImageCreateInfo const& create_info) : _device(&dev)
{
    VmaAllocationCreateInfo cie_spectrum_alloc_create{};
    cie_spectrum_alloc_create.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    vmaCreateImage(_device->get_allocator(), reinterpret_cast<VkImageCreateInfo const*>(&create_info), &cie_spectrum_alloc_create,
                   reinterpret_cast<VkImage*>(&_image), &_alloc, nullptr);
}

image::~image()
{
    if (_image && _device && _alloc) vmaDestroyImage(_device->get_allocator(), _image, _alloc);
}

image::image(image&& other) noexcept
{
    _image  = other._image;
    _alloc  = other._alloc;
    _device = other._device;

    other._image  = nullptr;
    other._alloc  = nullptr;
    other._device = nullptr;
}

image& image::operator=(image&& other) noexcept
{
    _image  = other._image;
    _alloc  = other._alloc;
    _device = other._device;

    other._image  = nullptr;
    other._alloc  = nullptr;
    other._device = nullptr;
    return *this;
}
}    // namespace v1
}    // namespace gfx