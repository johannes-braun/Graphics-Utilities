#pragma once
#include "shader.hpp"
#include "res/vertex.hpp"

namespace vkn
{
    struct GraphicsPipelineCreator
    {
        GraphicsPipelineCreator(vk::ArrayProxy<const jpu::ref_ptr<ShaderModule>> shaders, vk::PipelineLayout layout, vk::RenderPass renderpass, uint32_t subpass = 0);
        ~GraphicsPipelineCreator() = default;

        vk::GraphicsPipelineCreateInfo assemble() const;

        static vk::PipelineVertexInputStateCreateInfo defaultInputState()
        {
            static const auto attributes = {
                vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(res::vertex, position)),
                vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(res::vertex, uv)),
                vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(res::vertex, normal)),
            };
            static const auto bindings = { vk::VertexInputBindingDescription(0, sizeof(res::vertex)) };
            return vk::PipelineVertexInputStateCreateInfo({}, static_cast<uint32_t>(std::size(bindings)), std::data(bindings), static_cast<uint32_t>(std::size(attributes)), std::data(attributes));
        }
        
        vk::PipelineLayout layout;
        vk::RenderPass renderpass;
        uint32_t subpass;
        vk::PipelineCreateFlags flags;
        vk::PipelineVertexInputStateCreateInfo vertex_input = defaultInputState();
        vk::PipelineInputAssemblyStateCreateInfo input_assembly;
        std::vector<jpu::ref_ptr<ShaderModule>> shader_modules;
        vk::PipelineRasterizationStateCreateInfo rasterizer;
        vk::PipelineMultisampleStateCreateInfo multisample;
        std::vector<vk::DynamicState> dynamic_states = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
        };
        vk::PipelineDynamicStateCreateInfo dynamic_state;
        vk::PipelineViewportStateCreateInfo viewport_state;
        std::vector<vk::PipelineColorBlendAttachmentState> blend_attachments;
        vk::PipelineColorBlendStateCreateInfo blend_state;
        vk::PipelineDepthStencilStateCreateInfo depth_stencil;

    private:
        mutable std::vector<vk::PipelineShaderStageCreateInfo> m_shader_set;
    };
}
