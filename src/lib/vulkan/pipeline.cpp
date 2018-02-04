#include "pipeline.hpp"

namespace vkn
{
    pipeline_layout::pipeline_layout(device* device) : _device(device)
    {
        _device->inc_ref();
    }

    pipeline_layout::~pipeline_layout()
    {
        if (_layout)
            _device->destroyPipelineLayout(_layout);
        for (auto&& l : _descriptor_set_layouts)
            l->dec_ref();
        _device->dec_ref();
    }

    void pipeline_layout::add_push_constant_range(vk::PushConstantRange range)
    {
        _push_constant_range.push_back(range);
    }

    void pipeline_layout::add_push_constant_range(vk::ShaderStageFlags flags, uint32_t offset, uint32_t size)
    {
        _push_constant_range.emplace_back(flags, offset, size);
    }

    void pipeline_layout::add_descriptor_set_layout(descriptor_set_layout* layout)
    {
        _descriptor_set_layouts.push_back(layout);
        layout->inc_ref();
    }

    void pipeline_layout::finalize()
    {
        if (_layout)
            _device->destroyPipelineLayout(_layout);
        std::vector<vk::DescriptorSetLayout> lo(_descriptor_set_layouts.size());
        for (int i = 0; i < lo.size(); ++i)
            lo[i] = *_descriptor_set_layouts[i];
        _layout = _device->createPipelineLayout(vk::PipelineLayoutCreateInfo(
            {}, static_cast<uint32_t>(std::size(lo)), std::data(lo),
            static_cast<uint32_t>(std::size(_push_constant_range)), std::data(_push_constant_range)));
    }

    pipeline_layout::operator vk::PipelineLayout() const
    {
        return _layout;
    }

    graphics_pipeline::
        graphics_pipeline(device* device, pipeline_layout* layout, vk::RenderPass pass, uint32_t subpass) : layout(layout),
        renderpass(pass),
        subpass(subpass),
        _device(device)
    {
        _device->inc_ref();
        this->layout->inc_ref();

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
        color_blend_attachment.setColorWriteMask(
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::
            ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
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

    graphics_pipeline::~graphics_pipeline()
    {
        if (_pipeline) _device->destroyPipeline(_pipeline);
        for (auto&& sh : _shaders)
            sh->dec_ref();
        layout->dec_ref();
        _device->dec_ref();
    }

    void graphics_pipeline::set_vertex_attributes(const std::vector<vk::VertexInputAttributeDescription>& attribs)
    {
        _vertex_attributes = attribs;
    }

    void graphics_pipeline::set_vertex_bindings(const std::vector<vk::VertexInputBindingDescription>& bindings)
    {
        _vertex_bindings = bindings;
    }

    void graphics_pipeline::finalize()
    {
        _device->waitIdle();
        if (_pipeline) _device->destroyPipeline(_pipeline);
        _vertex_input.setPVertexAttributeDescriptions(std::data(_vertex_attributes)).setVertexAttributeDescriptionCount(
            static_cast<uint32_t>(std::size(_vertex_attributes))).
            setPVertexBindingDescriptions(std::data(_vertex_bindings)).setVertexBindingDescriptionCount(
                static_cast<uint32_t>(std::size(_vertex_bindings)));
        vk::GraphicsPipelineCreateInfo graphics_pipeline_info;
        graphics_pipeline_info.setFlags(flags)
            .setLayout(*layout)
            .setPVertexInputState(&_vertex_input)
            .setPInputAssemblyState(&input_assembly)
            .setStageCount(static_cast<uint32_t>(std::size(_stage_infos)))
            .setPStages(std::data(_stage_infos))
            .setPRasterizationState(&rasterizer)
            .setPMultisampleState(&multisample)
            .setPDynamicState(&dynamic_state)
            .setPViewportState(&viewport_state)
            .setPColorBlendState(&blend_state)
            .setPDepthStencilState(&depth_stencil)
            .setRenderPass(renderpass)
            .setSubpass(subpass);
        _pipeline = _device->createGraphicsPipeline(nullptr, graphics_pipeline_info);
    }

    void graphics_pipeline::reload_stages(bool force)
    {
        _device->waitIdle();
        if (_pipeline) _device->destroyPipeline(_pipeline);
        _pipeline = nullptr;
        for (int i=0; i<_shaders.size(); ++i)
        {
            _shaders[i]->reload(force);
            _stage_infos[i].stage = _shaders[i]->type();
            _stage_infos[i].module = *_shaders[i];
        }
        finalize();
    }

    graphics_pipeline::operator vk::Pipeline() const
    {
        return _pipeline;
    }
}
