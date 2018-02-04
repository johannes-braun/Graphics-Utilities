#include "pipeline.hpp"

namespace vkn
{
    GraphicsPipeline::GraphicsPipeline(const GraphicsPipelineCreateInfo& info)
        : ClassInfo(info), settings([&info]() {
        std::vector<jpu::ref_ptr<ShaderModule>> modules;
        modules.reserve(info.shader_paths.size());
        for (auto&& path : info.shader_paths)
            modules.push_back(jpu::make_ref<ShaderModule>(ShaderModuleCreateInfo(info.device, path)));
        return modules;
    }(), m_info.layout, m_info.renderpass, m_info.subpass)
    {
        m_info.device->inc_ref();

        rebuild();
    }

    GraphicsPipeline::~GraphicsPipeline()
    {
        if (m_pipeline) m_info.device->destroyPipeline(m_pipeline);
        m_info.device->dec_ref();
    }

    void GraphicsPipeline::rebuild(const bool reload_shaders)
    {
        m_info.device->waitIdle();
        if (m_pipeline) m_info.device->destroyPipeline(m_pipeline);

        if (reload_shaders)
        {
            settings.shader_modules.clear();
            settings.shader_modules.reserve(m_info.shader_paths.size());
            for (auto&& path : m_info.shader_paths)
                settings.shader_modules.push_back(jpu::make_ref<ShaderModule>(ShaderModuleCreateInfo(m_info.device, path)));
        }
        m_pipeline = m_info.device->createGraphicsPipeline(nullptr, settings.assemble());
    }

    void GraphicsPipeline::setRenderPass(const vk::RenderPass renderpass)
    {
        settings.renderpass = renderpass;
        m_info.setRenderPass(renderpass);
    }

    GraphicsPipeline::operator vk::Pipeline() const
    {
        return m_pipeline;
    }
}
