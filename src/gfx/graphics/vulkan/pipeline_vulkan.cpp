#include "pipeline_vulkan.hpp"
#include "context_vulkan.hpp"
#include "init_struct.hpp"

namespace gfx {
inline namespace v1 {
namespace vulkan {

graphics_pipeline_implementation::~graphics_pipeline_implementation() {}

bool graphics_pipeline_implementation::bind(vertex_input& input, state_info& info,
                                            const std::vector<std::pair<shader_type, std::shared_ptr<shader>>>& shaders, bool shaders_valid)
{
	return false;
}

std::any graphics_pipeline_implementation::api_handle() { return VkPipeline(_pipeline); }

compute_pipeline_implementation::~compute_pipeline_implementation() {}

bool compute_pipeline_implementation::build(shader& shaders)
{
	auto&                    ctx = context::current();
	auto impl = static_cast<context_implementation*>(std::any_cast<detail::context_implementation*>(ctx->implementation()));
	_device = impl->device();

	assert(shaders.format() == shader_format::spirv);

	init<VkShaderModuleCreateInfo> mc{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	mc.codeSize = static_cast<u32>(shaders.data().size());
	mc.pCode = reinterpret_cast<const uint32_t*>(shaders.data().data());

	VkShaderModule sm = nullptr;
	vkCreateShaderModule(_device, &mc, nullptr, &sm);

	init<VkComputePipelineCreateInfo> info{ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
	info.layout ={};
	info.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	info.stage.module = sm;
	info.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vkCreateComputePipelines(_device, nullptr, 1, &info, nullptr, &_pipeline);

	vkDestroyShaderModule(_device, sm, nullptr);

	return true;
}

std::any compute_pipeline_implementation::api_handle() { return VkPipeline(_pipeline); }
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx