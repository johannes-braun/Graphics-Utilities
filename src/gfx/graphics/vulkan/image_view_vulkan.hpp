#pragma once

#include "../general/handle.hpp"
#include "../image_view.hpp"
#include <vulkan/vulkan.h>

namespace gfx {
inline namespace v1 {
namespace vulkan {
class image_view_implementation : public detail::image_view_implementation
{
public:
    ~image_view_implementation() override;
    void initialize(imgv_type type, format format, const device_image& image, uint32_t base_mip, uint32_t mip_count, uint32_t base_layer,
                    uint32_t layer_count) override;
    std::any api_handle() override;


    void initialize_vk(imgv_type type, format format, const VkImage& image, uint32_t base_mip, uint32_t mip_count, uint32_t base_layer,
                       uint32_t layer_count);

    VkImageView view() const noexcept { return _view; }

private:
    VkDevice                    _device = nullptr;
    movable_handle<VkImageView> _view;
};
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx
