#pragma once

#include <filesystem>
#include <vulkan/vulkan.hpp>
#include <jpu/memory>
#include "device.hpp"
#include "create_info.hpp"
#include "binshader/shader.hpp"

namespace vkn
{
    namespace
    {
        std::experimental::filesystem::path shader_root_path = "../shaders";
        std::vector<std::experimental::filesystem::path> shader_include_directories{
            shader_root_path
        };
        const std::experimental::filesystem::path& shader_root = shader_root_path;
    }


    inline void setup_shader_paths(std::experimental::filesystem::path root, const std::vector<std::experimental::filesystem::path>& include_directories = {})
    {
        shader_root_path = root;
        shader_include_directories.clear();
        shader_include_directories.push_back(root);
        shader_include_directories.insert(shader_include_directories.end(), include_directories.begin(), include_directories.end());
    }

    class shader : public jpu::ref_count
    {
    public:
        explicit shader(device* device, vk::ShaderStageFlagBits type, const std::experimental::filesystem::path& path, const std::vector<res::definition>& definitions = {});
        explicit shader(device* device, const std::experimental::filesystem::path& path, const std::vector<res::definition>& definitions = {});

        ~shader();

        vk::ShaderStageFlagBits type() const;
        void reload(bool force = false);
        void reload(const std::vector<res::definition>& definitions);
        operator vk::ShaderModule() const;
    private:
        static vk::ShaderStageFlagBits type_of(const std::experimental::filesystem::path& extension)
        {
            if (extension == ".vert")
                return vk::ShaderStageFlagBits::eVertex;
            if (extension == ".frag")
                return vk::ShaderStageFlagBits::eFragment;
            if (extension == ".geom")
                return vk::ShaderStageFlagBits::eGeometry;
            if (extension == ".tesc")
                return vk::ShaderStageFlagBits::eTessellationControl;
            if (extension == ".tese")
                return vk::ShaderStageFlagBits::eTessellationEvaluation;
            if (extension == ".comp")
                return vk::ShaderStageFlagBits::eCompute;

            throw std::invalid_argument("File extension " + extension.string() + " does not refer to any shader type.");
        }

        device* _device;
        vk::ShaderModule _shader_module;
        vk::ShaderStageFlagBits _type;
        std::experimental::filesystem::path _path;
        std::vector<res::definition> _definitions;
    };








    namespace fs = std::experimental::filesystem;

   /* struct ShaderModuleCreateInfo
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
    };*/
}
