#include "shader.hpp"
#include "device.hpp"
#include <gfx.core/log.hpp>
#include <fstream>

namespace gfx {
inline namespace v1 {
shader::shader(device& dev, gsl::span<const u32> source)
{
	load(dev.get_device(), source);
}

const vk::ShaderModule& shader::get_module() const noexcept
{
	return _module.get();
}

void shader::load(vk::Device dev, gsl::span<const u32> source)
{
	vk::ShaderModuleCreateInfo create;
	create.codeSize = source.size_bytes();
	create.pCode = source.data();
	_module = dev.createShaderModuleUnique(create);
}
}    // namespace v1
}    // namespace gfx