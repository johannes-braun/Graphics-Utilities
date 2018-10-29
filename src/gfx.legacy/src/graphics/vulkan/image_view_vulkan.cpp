#include "device_image_vulkan.hpp"
#include "image_view_vulkan.hpp"
#include "init_struct.hpp"
#include "context_vulkan.hpp"
#include "result.hpp"

namespace gfx {
inline namespace v1 {
namespace vulkan {
image_view_implementation::~image_view_implementation()
{
	if (_view) vkDestroyImageView(_device, _view, nullptr);
}


void image_view_implementation::initialize(imgv_type type, format format, const device_image& image, uint32_t base_mip, uint32_t mip_count,
	uint32_t base_layer, uint32_t layer_count)
{
	initialize_vk(type, format, handle_cast<VkImage>(image), base_mip, mip_count, base_layer, layer_count);
}

void image_view_implementation::initialize_vk(imgv_type type, format format, const VkImage& image, uint32_t base_mip, uint32_t mip_count,
                                           uint32_t base_layer, uint32_t layer_count)
{
	auto&                    ctx = context::current();
	auto impl = static_cast<context_implementation*>(std::any_cast<detail::context_implementation*>(ctx->implementation()));
	_device = impl->device();

    init<VkImageViewCreateInfo> create_info{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    create_info.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                              VK_COMPONENT_SWIZZLE_IDENTITY};
    create_info.format     = get_format(format);

    switch (format)
    {
    case d16unorm:
    case d24unorm:
    case d32f: create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT; break;
    case s8ui: create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT; break;
    case d24unorms8ui:
    case d32fs8ui: create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT; break;
    default: create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; break;
    }

	create_info.subresourceRange.baseArrayLayer = base_layer;
	create_info.subresourceRange.baseMipLevel = base_mip;
	create_info.subresourceRange.layerCount = layer_count;
	create_info.subresourceRange.levelCount = mip_count;
	create_info.image = image;
    create_info.viewType = [&]() -> VkImageViewType {
        switch(type)
        {
        case imgv_type::image1d: return VK_IMAGE_VIEW_TYPE_1D;
        case imgv_type::image2d: return VK_IMAGE_VIEW_TYPE_2D;
        case imgv_type::image3d: return VK_IMAGE_VIEW_TYPE_3D;
        case imgv_type::image_cube: return VK_IMAGE_VIEW_TYPE_CUBE;
        case imgv_type::image1d_array: return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
        case imgv_type::image2d_array: return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        case imgv_type::image_cube_array: return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
        default: return VK_IMAGE_VIEW_TYPE_2D;
        }
	}();

	check_result(vkCreateImageView(_device, &create_info, nullptr, &_view));
}

std::any image_view_implementation::api_handle()
{
    return VkImageView(_view);
}
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx