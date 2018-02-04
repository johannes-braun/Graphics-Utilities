#pragma once

#include <filesystem>
#include <vulkan/vulkan.hpp>
#include <jpu/memory>
#include "device.hpp"
#include "create_info.hpp"

namespace vkn
{
    namespace fs = std::experimental::filesystem;

    struct ShaderModuleCreateInfo
    {
        ShaderModuleCreateInfo(device* device, const fs::path path)
            : device(device), path(path) {}

        ShaderModuleCreateInfo& setDevice(device* value) { device = value; return *this; }
        ShaderModuleCreateInfo& setPath(const fs::path value) { path = value; return *this; }

        device* device;
        fs::path path;
    };

    class ShaderModule : ClassInfo<ShaderModuleCreateInfo, ShaderModule>, public jpu::ref_count, public vk::ShaderModule
    {
    public:
        explicit ShaderModule(const ShaderModuleCreateInfo& info);
        ~ShaderModule();

        vk::PipelineShaderStageCreateInfo makePipelineStage(vk::ShaderStageFlagBits override_stage = vk::ShaderStageFlagBits(0), const char* override_entry = "main") const;

    private:
        vk::ShaderStageFlagBits m_proposed_stage;
    };
}
