#pragma once

#include "../device_image.hpp"
#include "../general/handle.hpp"
#include <vulkan/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace gfx {
inline namespace v1 {
namespace vulkan {
VkFormat get_format(format f);

class device_image_implementation : public detail::device_image_implementation
{
public:
    ~device_image_implementation() override;

    void     initialize(uint32_t layer_dimensions, format format, const extent& size, uint32_t levels, sample_count samples) override;
    void     fill_from(const host_image& image, uint32_t level, uint32_t layer) override;
    void     fill_to(const host_image& image, uint32_t level, uint32_t layer) override;
    std::any api_handle() override;
    void     generate_mipmaps() override;

private:
    VkDevice                _device = nullptr;
    movable_handle<VkImage> _image  = nullptr;
    VkExtent3D              _extent = {1, 1, 1};
	u32 _levels = 0;
	u32 _layers = 0;
	movable_handle<VkFence> _transfer_fence = nullptr;

    VmaAllocator                  _allocator  = nullptr;
    movable_handle<VmaAllocation> _allocation = nullptr;

	VkQueue _transfer_queue = nullptr;
	VkQueue _graphics_queue = nullptr;
    VkCommandPool _transfer_pool = nullptr;
	VkCommandPool _graphics_pool = nullptr;
};

}    // namespace vulkan
}    // namespace v1
}    // namespace gfx