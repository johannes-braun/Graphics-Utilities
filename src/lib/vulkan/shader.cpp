#include "shader.hpp"
#include "xspirv/xspirv.hpp"

namespace vkn
{
    ShaderModule::ShaderModule(const ShaderModuleCreateInfo& info)
        : ClassInfo(info), vk::ShaderModule([&]() {
            auto binary = xspirv::load_file(info.path, { "../shaders/" }, {});
            return info.device->createShaderModule(vk::ShaderModuleCreateInfo({}, binary.size() * sizeof(uint32_t), binary.data()));
        }())
    {
        m_info.device->inc_ref();

        const auto extension = m_info.path.extension();
        if (extension == ".vert")
            m_proposed_stage = vk::ShaderStageFlagBits::eVertex;
        else if (extension == ".frag")
            m_proposed_stage = vk::ShaderStageFlagBits::eFragment;
        else if (extension == ".geom")
            m_proposed_stage = vk::ShaderStageFlagBits::eGeometry;
        else if (extension == ".tesc")
            m_proposed_stage = vk::ShaderStageFlagBits::eTessellationControl;
        else if (extension == ".tese")
            m_proposed_stage = vk::ShaderStageFlagBits::eTessellationEvaluation;
    }

    ShaderModule::~ShaderModule()
    {
        m_info.device->destroyShaderModule(*this);
        m_info.device->dec_ref();
    }

    vk::PipelineShaderStageCreateInfo ShaderModule::makePipelineStage(const vk::ShaderStageFlagBits override_stage,
        const char* override_entry) const
    {
        auto stage = m_proposed_stage;
        if (override_stage != vk::ShaderStageFlagBits(0))
            stage = override_stage;
        return vk::PipelineShaderStageCreateInfo({}, stage, *this, override_entry);
    }
}
