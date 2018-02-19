#pragma once

#include "device.hpp"
#include "shader.hpp"
#include "descriptor.hpp"

#include <jpu/memory>
#include <vulkan/vulkan.hpp>

namespace vkn
{
    class pipeline_layout : public jpu::ref_count
    {
    public:
        pipeline_layout(device* device);
        ~pipeline_layout();

        void add_push_constant_range(vk::PushConstantRange range);
        void add_push_constant_range(vk::ShaderStageFlags flags, uint32_t offset, uint32_t size);
        void add_descriptor_set_layout(descriptor_set_layout* layout);
        void finalize();

        operator vk::PipelineLayout() const;

    private:
        device * _device;
        vk::PipelineLayout _layout;
        std::vector<vk::PushConstantRange> _push_constant_range;
        std::vector<descriptor_set_layout*> _descriptor_set_layouts;
    };

    class graphics_pipeline : public jpu::ref_count
    {
    public:
        graphics_pipeline(device* device, pipeline_layout* layout, vk::RenderPass pass, uint32_t subpass = 0);
        ~graphics_pipeline();

        template<typename... TShaders>
        void use_stages(TShaders ... shd);
        void set_vertex_attributes(const std::vector<vk::VertexInputAttributeDescription>& attribs);
        void set_vertex_bindings(const std::vector<vk::VertexInputBindingDescription>& bindings);
        void finalize();

        void reload_stages(bool force = false);

        operator vk::Pipeline() const;

        pipeline_layout* layout;
        vk::RenderPass renderpass;
        uint32_t subpass;
        vk::PipelineCreateFlags flags;
        vk::PipelineInputAssemblyStateCreateInfo input_assembly;
        vk::PipelineRasterizationStateCreateInfo rasterizer;
        vk::PipelineMultisampleStateCreateInfo multisample;
        std::vector<vk::DynamicState> dynamic_states = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
        };
        vk::PipelineViewportStateCreateInfo viewport_state;
        std::vector<vk::PipelineColorBlendAttachmentState> blend_attachments;
        vk::PipelineColorBlendStateCreateInfo blend_state;
        vk::PipelineDepthStencilStateCreateInfo depth_stencil;

    private:
        vk::PipelineDynamicStateCreateInfo _dynamic_state;
        std::vector<vk::VertexInputAttributeDescription> _vertex_attributes;
        std::vector<vk::VertexInputBindingDescription> _vertex_bindings;
        vk::PipelineVertexInputStateCreateInfo _vertex_input;
        vk::Pipeline _pipeline;
        device* _device;
        std::vector<shader*> _shaders;
        std::vector<vk::PipelineShaderStageCreateInfo> _stage_infos;
    };

    template <typename ... TShaders>
    void graphics_pipeline::use_stages(TShaders ... shd)
    {
        for (auto&& sh : _shaders)
            sh->dec_ref();
        _shaders.clear();
        const std::initializer_list<shader*> list{ shd... };
        for (auto s : list)
        {
            s->inc_ref();
            _shaders.push_back(s);
            _stage_infos.emplace_back(vk::PipelineShaderStageCreateFlags(), s->type(), *s, "main");
        }
    }
}
