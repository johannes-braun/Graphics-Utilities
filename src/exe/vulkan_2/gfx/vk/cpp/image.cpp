#include "../image.hpp"

#include "../device.hpp"

namespace gfx::vk
{
    image::~image()
    {
        if (_owning)
            vkDestroyImage(*_device, _image, nullptr);
    }
}