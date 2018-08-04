#include "vertex_input_vulkan.hpp"
#include "device_image_vulkan.hpp"

namespace gfx {
inline namespace v1 {
namespace vulkan {

vertex_input_implementation::vertex_input_implementation()
{
    _input_assembly.primitiveRestartEnable = false;
    _input_assembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    _vertex_input.vertexBindingDescriptionCount   = 0;
    _vertex_input.vertexAttributeDescriptionCount = 0;
    _vertex_input.pVertexAttributeDescriptions    = nullptr;
    _vertex_input.pVertexBindingDescriptions      = nullptr;
}

uint32_t vertex_input_implementation::add_attribute(uint32_t binding, gfx::format fmt, size_t offset)
{
	VkVertexInputAttributeDescription& attr = _attributes.emplace_back();
	attr.binding = binding;
	attr.location = static_cast<u32>(_attributes.size()-1);
	attr.format = get_format(fmt);
	attr.offset = offset;
	return attr.location;
}

void vertex_input_implementation::set_binding_info(uint32_t binding, size_t stride, input_rate rate)
{
    VkVertexInputBindingDescription& description = [this, binding]() -> VkVertexInputBindingDescription& {
        if (const auto it = std::find_if(_bindings.begin(), _bindings.end(),
                                         [binding](const VkVertexInputBindingDescription& d) { return d.binding == binding; });
            it != _bindings.end())
            return *it;
        return _bindings.emplace_back();
    }();
    description.binding = binding;
    switch (rate)
    {
    case input_rate::vertex: description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; break;
    case input_rate::instance: description.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE; break;
    }
    description.stride = stride;
}

void vertex_input_implementation::set_assembly(topology mode, bool enable_primitive_restart)
{
    _input_assembly.primitiveRestartEnable = enable_primitive_restart;
    switch (mode)
    {
    case topology::point_list: _input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST; break;
    case topology::line_list: _input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; break;
    case topology::line_strip: _input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; break;
    case topology::triangle_list: _input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
    case topology::triangle_strip: _input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; break;
    case topology::triangle_fan: _input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN; break;
    case topology::line_list_adj: _input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY; break;
    case topology::line_strip_adj: _input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY; break;
    case topology::triangle_list_adj: _input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY; break;
    case topology::triangle_strip_adj: _input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY; break;
    case topology::patch_list: _input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST; break;
    }
}

std::any vertex_input_implementation::api_handle()
{
    return std::pair<VkPipelineInputAssemblyStateCreateInfo*, VkPipelineVertexInputStateCreateInfo*>(&_input_assembly, &_vertex_input);
}
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx