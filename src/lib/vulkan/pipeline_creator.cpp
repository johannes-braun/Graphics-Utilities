#include "pipeline_creator.hpp"

namespace vkn
{
    GraphicsPipelineCreator::GraphicsPipelineCreator(vk::ArrayProxy<const jpu::ref_ptr<ShaderModule>> shaders, const vk::PipelineLayout layout, const vk::RenderPass renderpass, const uint32_t subpass)
        : layout(layout), renderpass(renderpass), subpass(subpass), shader_modules(shaders.begin(), shaders.end())
    {
        // Create a vertex input state
        // The default attribute locations and size for a vtx::Vertex are being used.

        // Render the geometry as triangles
        input_assembly.setTopology(vk::PrimitiveTopology::eTriangleList);

        // Use a simple rasterizer with filled, ccw-triangles and backface-culling.
        rasterizer.setPolygonMode(vk::PolygonMode::eFill)
            .setLineWidth(1.f)
            .setCullMode(vk::CullModeFlagBits::eBack)
            .setFrontFace(vk::FrontFace::eCounterClockwise);

        // Don't use MSAA... For now...
        multisample.setSampleShadingEnable(true)
            .setRasterizationSamples(vk::SampleCountFlagBits::e1)
            .setMinSampleShading(0.25f);

        // Adapt to screen. This is needed even though I'll have the viewport and scissor as dynamic states
        viewport_state.setViewportCount(1).setScissorCount(1);

        // Add some additive color blending for improved "fanciness".
        vk::PipelineColorBlendAttachmentState color_blend_attachment;
        color_blend_attachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(false)
            .setAlphaBlendOp(vk::BlendOp::eAdd)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eOne)
            .setSrcColorBlendFactor(vk::BlendFactor::eOne)
            .setDstColorBlendFactor(vk::BlendFactor::eOne);
        blend_attachments = { color_blend_attachment };
        blend_state.setLogicOpEnable(true)
            .setLogicOp(vk::LogicOp::eCopy)
            .setAttachmentCount(static_cast<uint32_t>(std::size(blend_attachments)))
            .setPAttachments(std::data(blend_attachments));

        // Depth and stencil masking. 
        depth_stencil.setDepthTestEnable(true)
            .setDepthWriteEnable(true)
            .setDepthCompareOp(vk::CompareOp::eGreater)
            .setDepthBoundsTestEnable(true)
            .setMinDepthBounds(0.f)
            .setMaxDepthBounds(1.f);

        // As said before, use the viewport and scissor as dynamic states.
        dynamic_state.setDynamicStateCount(static_cast<uint32_t>(std::size(dynamic_states)))
            .setPDynamicStates(std::data(dynamic_states));
    }

    vk::GraphicsPipelineCreateInfo GraphicsPipelineCreator::assemble() const
    {
        // These are the shaders to be used with this graphics pipeline.
        m_shader_set.clear();
        m_shader_set.reserve(shader_modules.size());
        for(const auto& shader : shader_modules)
            m_shader_set.push_back(shader->makePipelineStage());

        // Aaand create...
        vk::GraphicsPipelineCreateInfo graphics_pipeline_info;
        graphics_pipeline_info.setFlags(flags)
            .setLayout(layout)
            .setPVertexInputState(&vertex_input)
            .setPInputAssemblyState(&input_assembly)
            .setStageCount(static_cast<uint32_t>(std::size(m_shader_set)))
            .setPStages(std::data(m_shader_set))
            .setPRasterizationState(&rasterizer)
            .setPMultisampleState(&multisample)
            .setPDynamicState(&dynamic_state)
            .setPViewportState(&viewport_state)
            .setPColorBlendState(&blend_state)
            .setPDepthStencilState(&depth_stencil)
            .setRenderPass(renderpass)
            .setSubpass(subpass);
        return graphics_pipeline_info;
    }
}
