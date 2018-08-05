#include "graphics_pipeline_vulkan.hpp"
#include <gfx/log.hpp>
#include "context_vulkan.hpp"
#include "init_struct.hpp"

namespace gfx {
namespace v2 {
namespace vulkan {
    graphics_pipeline_implementation::~graphics_pipeline_implementation()
{
}

void graphics_pipeline_implementation::initialize(const pipeline_state& state)
{
	auto&                    ctx = context::current();
	auto impl = static_cast<v1::vulkan::context_implementation*>(std::any_cast<v1::detail::context_implementation*>(ctx->implementation()));
	_device   = impl->device();

	v1::vulkan::init<VkPipelineLayoutCreateInfo> pl_info { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };

	std::vector<VkDescriptorSetLayout> layouts;
    if(state.state_bindings)
    {
		for (auto& bl : state.state_bindings->binding_layouts)
			layouts.push_back(handle_cast<VkDescriptorSetLayout>(*bl));
    }
	pl_info.setLayoutCount = layouts.size();
	pl_info.pSetLayouts = layouts.data();
	vkCreatePipelineLayout(_device, &pl_info, nullptr, &_layout);

	v1::vulkan::init<VkGraphicsPipelineCreateInfo> pp_info { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	pp_info.layout = _layout;
}

handle graphics_pipeline_implementation::api_handle()
{
}
}    // namespace vulkan
}    // namespace v2
}    // namespace gfx