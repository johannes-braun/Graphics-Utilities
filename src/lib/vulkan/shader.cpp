#include "shader.hpp"

namespace vkn
{
    void setup_shader_paths(const std::experimental::filesystem::path root,
                            const std::vector<std::experimental::filesystem::path>& include_directories)
    {
        shader_root_path = root;
        shader_include_directories.clear();
        shader_include_directories.push_back(root);
        shader_include_directories.insert(shader_include_directories.end(), include_directories.begin(),
                                          include_directories.end());
    }

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

    void shader::reload(bool /*force*/)
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

    vk::ShaderStageFlagBits shader::type_of(const std::experimental::filesystem::path& extension)
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
}
