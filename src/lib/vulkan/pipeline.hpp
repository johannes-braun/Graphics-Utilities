#pragma once

#include <vulkan/vulkan.hpp>
#include <jpu/memory>
#include "device.hpp"
#include "pipeline_creator.hpp"

namespace vkn
{
    enum class GraphicsPipelineCreateFlagBits
    {
    };
    using GraphicsPipelineCreateFlags = vk::Flags<GraphicsPipelineCreateFlagBits, uint32_t>;

    struct GraphicsPipelineCreateInfo
    {
        GraphicsPipelineCreateInfo(
            const GraphicsPipelineCreateFlags flags = {},
            device* device = nullptr,
            const vk::PipelineLayout layout = nullptr,
            const vk::RenderPass renderpass = nullptr,
            const uint32_t subpass = 0,
            const vk::ArrayProxy<const fs::path> shader_paths = {})
            : flags(flags), device(device), layout(layout), renderpass(renderpass), subpass(subpass), shader_paths(shader_paths.begin(), shader_paths.end()) {}
        
        GraphicsPipelineCreateInfo& setFlags(const GraphicsPipelineCreateFlags value) { flags = value; return *this; }
        GraphicsPipelineCreateInfo& setDevice(device* value) { device = value; return *this; }
        GraphicsPipelineCreateInfo& setLayout(const vk::PipelineLayout value) { layout = value; return *this; }
        GraphicsPipelineCreateInfo& setRenderPass(const vk::RenderPass value) { renderpass = value; return *this; }
        GraphicsPipelineCreateInfo& setSubPass(const uint32_t value) { subpass = value; return *this; }
        GraphicsPipelineCreateInfo& setShaderPaths(const vk::ArrayProxy<const fs::path> value) { shader_paths = std::vector<fs::path>(value.begin(), value.end()); return *this; }

        GraphicsPipelineCreateFlags flags;
        device* device;
        vk::PipelineLayout layout;
        vk::RenderPass renderpass;
        uint32_t subpass;
        std::vector<fs::path> shader_paths;
    };

    class GraphicsPipeline : ClassInfo<GraphicsPipelineCreateInfo, GraphicsPipeline>, public jpu::ref_count
    {
    public:
        GraphicsPipelineCreator settings;

        explicit GraphicsPipeline(const GraphicsPipelineCreateInfo& info);
        ~GraphicsPipeline();

        void rebuild(bool reload_shaders = false);
        
        void setRenderPass(vk::RenderPass renderpass);
        operator vk::Pipeline() const;

    private:
        vk::Pipeline m_pipeline;
    };
}
