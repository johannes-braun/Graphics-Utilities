#include "sampler_vulkan.hpp"
#include "context_vulkan.hpp"
#include "result.hpp"

namespace gfx {
inline namespace v1 {
namespace vulkan {

sampler_implementation::sampler_implementation()
{
    _create_info.magFilter               = VK_FILTER_LINEAR;
    _create_info.minFilter               = VK_FILTER_LINEAR;
    _create_info.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    _create_info.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    _create_info.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    _create_info.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    _create_info.mipLodBias              = 0.f;
    _create_info.anisotropyEnable        = true;
    _create_info.maxAnisotropy           = 16.f;
    _create_info.compareEnable           = false;
    _create_info.compareOp               = VK_COMPARE_OP_NEVER;
    _create_info.minLod                  = -1000.f;
    _create_info.maxLod                  = 1000.f;
    _create_info.borderColor             = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    _create_info.unnormalizedCoordinates = false;
}


sampler_implementation::~sampler_implementation()
{
    if (_sampler) vkDestroySampler(_device, _sampler, nullptr);
    _sampler = nullptr;
}

void sampler_implementation::set_filter(filter_mode mode, filter filter)
{
    invalidate_handle();
    const auto get_filter = [filter] {
        switch (filter)
        {
        case filter::nearest: return VK_FILTER_NEAREST;
        case filter::linear: return VK_FILTER_LINEAR;
        default: return VK_FILTER_LINEAR;
        }
    };
    switch (mode)
    {
    case filter_mode::mag: _create_info.magFilter = get_filter(); break;
    case filter_mode::min: _create_info.minFilter = get_filter(); break;
    case filter_mode::mipmap:
        _create_info.mipmapMode = [filter] {
            switch (filter)
            {
            case filter::nearest: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
            case filter::linear: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
            default: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
            }
        }();
        break;
    default:;
    }
}

void sampler_implementation::set_wrap(wrap w, wrap_mode mode)
{
    invalidate_handle();

    VkSamplerAddressMode vkm = [mode]() {
        switch (mode)
        {
        case wrap_mode::repeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case wrap_mode::mirror_repeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case wrap_mode::clamp_to_edge: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case wrap_mode::clamp_to_border: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case wrap_mode::mirror_clamp_to_edge: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        default: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }
    }();

    switch (w)
    {
    case wrap::u: _create_info.addressModeU = vkm; break;
    case wrap::v: _create_info.addressModeV = vkm; break;
    case wrap::w: _create_info.addressModeW = vkm; break;
    }
}

void sampler_implementation::set_border(border_color color)
{
    invalidate_handle();
    switch (color)
    {
    case border_color::float_transparent_black: _create_info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK; break;
    case border_color::int_transparent_black: _create_info.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK; break;
    case border_color::float_opaque_black: _create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK; break;
    case border_color::int_opaque_black: _create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK; break;
    case border_color::float_opaque_white: _create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE; break;
    case border_color::int_opaque_white: _create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE; break;
    default:;
    }
}

void sampler_implementation::set_lod(lod mode, float value)
{
    invalidate_handle();

    switch (mode)
    {
    case lod::bias: _create_info.mipLodBias = value; break;
    case lod::min: _create_info.minLod = value; break;
    case lod::max: _create_info.maxLod = value; break;
    default:;
    }
}

void sampler_implementation::set_anisotropy(bool enable, float value)
{
    invalidate_handle();
    _create_info.anisotropyEnable = enable;
    _create_info.maxAnisotropy    = value;
}

void sampler_implementation::set_compare(bool enable, compare_op op)
{
    invalidate_handle();
    _create_info.compareEnable = enable;
    switch (op)
    {
    case compare_op::never: _create_info.compareOp = VK_COMPARE_OP_NEVER; break;
    case compare_op::less: _create_info.compareOp = VK_COMPARE_OP_LESS; break;
    case compare_op::equal: _create_info.compareOp = VK_COMPARE_OP_EQUAL; break;
    case compare_op::less_or_equal: _create_info.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL; break;
    case compare_op::greater: _create_info.compareOp = VK_COMPARE_OP_GREATER; break;
    case compare_op::not_equal: _create_info.compareOp = VK_COMPARE_OP_NOT_EQUAL; break;
    case compare_op::greater_or_equal: _create_info.compareOp = VK_COMPARE_OP_GREATER_OR_EQUAL; break;
    case compare_op::always: _create_info.compareOp = VK_COMPARE_OP_ALWAYS; break;
    default:;
    }
}

std::any sampler_implementation::api_handle()
{
    if (!_sampler) init_handle();
    return VkSampler(_sampler);
}

void sampler_implementation::invalidate_handle()
{
    this->~sampler_implementation();
}

void sampler_implementation::init_handle()
{
	if (!_device)
	{
		auto& ctx  = context::current();
		auto  impl = static_cast<context_implementation*>(std::any_cast<detail::context_implementation*>(ctx->implementation()));
		_device = impl->device();
	}
	check_result(vkCreateSampler(_device, &_create_info, nullptr, &_sampler));
}
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx