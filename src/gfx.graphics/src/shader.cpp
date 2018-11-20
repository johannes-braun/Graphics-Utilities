#include "shader.hpp"
#include "device.hpp"
#include <fstream>
#include <gfx.core/log.hpp>

namespace gfx {
inline namespace v1 {
shader::shader(device& dev, gsl::span<u32 const> source)
{
    load(dev.get_device(), source);
}

const vk::ShaderModule& shader::get_module() const noexcept
{
    return _module.get();
}

void shader::load(vk::Device dev, gsl::span<u32 const> source)
{
    vk::ShaderModuleCreateInfo create;
    create.codeSize = source.size_bytes();
    create.pCode    = source.data();
    _module         = dev.createShaderModuleUnique(create);
}
}    // namespace v1
}    // namespace gfx