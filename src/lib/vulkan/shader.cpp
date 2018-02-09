#include "shader.hpp"

namespace vkn
{
    shader::shader(device* device, const vk::ShaderStageFlagBits type, const std::experimental::filesystem::path& path,
        const std::vector<glshader::definition>& definitions)
        : _device(device), _type(type), _path(path), _definitions(definitions)
    {
        _device->inc_ref();
        reload(true);
    }

    shader::shader(device* device, const std::experimental::filesystem::path& path,
        const std::vector<glshader::definition>& definitions)
        : shader(device, type_of(path.extension()), path, definitions)
    {
    }

    shader::~shader()
    {
        _device->destroyShaderModule(_shader_module);
        _device->dec_ref();
    }

    vk::ShaderStageFlagBits shader::type() const
    {
        return _type;
    }

    void shader::reload(bool force)
    {
        _device->waitIdle();
        if(_shader_module)
            _device->destroyShaderModule(_shader_module);

        auto bin = glshader::load_binary_shader(glshader::shader_format::spirv, _path, { "../shaders/" }, {});
        _shader_module = _device->createShaderModule(vk::ShaderModuleCreateInfo({}, bin.data.size(),
            reinterpret_cast<const uint32_t*>(bin.data.data())));
    }

    void shader::reload(const std::vector<glshader::definition>& definitions)
    {
        _definitions = definitions;
        reload(true);
    }

    shader::operator vk::ShaderModule() const
    {
        return _shader_module;
    }
}
