#include "binding_layout_vulkan.hpp"
#include "context_vulkan.hpp"
#include "result.hpp"

namespace gfx {
inline namespace v1 {
namespace vulkan {

binding_layout_implementation::~binding_layout_implementation()
{
    if (_layout) vkDestroyDescriptorSetLayout(_device, _layout, nullptr);
}

handle binding_layout_implementation::api_handle()
{
    if (!_device) {
        auto& ctx  = context::current();
        auto  impl = static_cast<context_implementation*>(std::any_cast<detail::context_implementation*>(ctx->implementation()));
        _device    = impl->device();
    }

    if (_invalidated) {
		if (_layout) vkDestroyDescriptorSetLayout(_device, _layout, nullptr);
		_create_info.flags = _dynamic ? VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR : 0;
		_create_info.bindingCount = static_cast<u32>(_bindings.size());
		_create_info.pBindings    = _bindings.data();
		check_result(vkCreateDescriptorSetLayout(_device, &_create_info, nullptr, &_layout));
		_invalidated = false;
    }

    return VkDescriptorSetLayout(_layout);
}

void binding_layout_implementation::push(binding_type type, u32 array_size = 1)
{
    init<VkDescriptorSetLayoutBinding> bnd;
    bnd.binding         = static_cast<u32>(_bindings.size());
    bnd.descriptorCount = array_size;
    bnd.descriptorType  = [type] {
        switch (type)
        {
        case binding_type::uniform_buffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case binding_type::storage_buffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case binding_type::sampled_image: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case binding_type::storage_image: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        default: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
    }();
	
    bnd.stageFlags = VK_SHADER_STAGE_ALL;
    _bindings.emplace_back(bnd);
    _invalidated              = true;
}

}    // namespace vulkan
}    // namespace v1
}    // namespace gfx