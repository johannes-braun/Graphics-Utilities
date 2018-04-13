#pragma once

#include "device.hpp"

#include <filesystem>
#include <vulkan/vulkan.hpp>
#include <jpu/memory.hpp>
#include <glshader/binary.hpp>

namespace vkn
{
    namespace
    {
        std::experimental::filesystem::path shader_root_path = "../shaders";
        std::vector<std::experimental::filesystem::path> shader_include_directories{ shader_root_path };
        const std::experimental::filesystem::path& shader_root = shader_root_path;
    }
    void setup_shader_paths(std::experimental::filesystem::path root, const std::vector<std::experimental::filesystem::path>& include_directories = {});

    class shader : public jpu::ref_count
    {
    public:
        explicit shader(device* device, vk::ShaderStageFlagBits type, const std::experimental::filesystem::path& path, const std::vector<glsp::definition>& definitions = {});
        explicit shader(device* device, const std::experimental::filesystem::path& path, const std::vector<glsp::definition>& definitions = {});

        ~shader();

        vk::ShaderStageFlagBits type() const;
        void reload(bool force = false);
        void reload(const std::vector<glsp::definition>& definitions);
        operator vk::ShaderModule() const;
    private:
        static vk::ShaderStageFlagBits type_of(const std::experimental::filesystem::path& extension);

        device* _device;
        vk::ShaderModule _shader_module;
        vk::ShaderStageFlagBits _type;
        std::experimental::filesystem::path _path;
        std::vector<glsp::definition> _definitions;
    };
}
