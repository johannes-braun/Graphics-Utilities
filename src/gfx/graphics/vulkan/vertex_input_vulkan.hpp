#pragma once

#include "../vertex_input.hpp"
#include "init_struct.hpp"
#include <vulkan/vulkan.h>

namespace gfx {
inline namespace v1 {
namespace vulkan {

class vertex_input_implementation : public detail::vertex_input_implementation
{
public:
    vertex_input_implementation();

    uint32_t add_attribute(uint32_t binding, gfx::format fmt, size_t offset) override;
    void     set_binding_info(uint32_t binding, size_t stride, input_rate rate) override;
    void     set_assembly(topology mode, bool enable_primitive_restart) override;
    std::any api_handle() override;

private:
    init<VkPipelineInputAssemblyStateCreateInfo> _input_assembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    init<VkPipelineVertexInputStateCreateInfo>   _vertex_input{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};

	std::vector<VkVertexInputAttributeDescription> _attributes;
	std::vector<VkVertexInputBindingDescription> _bindings;
};

}    // namespace vulkan
}    // namespace v1
}    // namespace gfx