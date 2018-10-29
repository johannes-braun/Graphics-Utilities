#include "shader.hpp"
#include "device.hpp"
#include <gfx.legacy/../../src/graphics/shader_include.hpp>
#include <gfx.core/log.hpp>
#include <fstream>

namespace gfx {
inline namespace v1 {
shader::shader(device& dev, const std::filesystem::path& path)
	: _path(path)
{
	load(dev.get_device());
}

shader::shader(const shader& other)
	: _path(other._path)
{
	load(other._module.getOwner());
}

shader& shader::operator=(const shader& other)
{
	_path = other._path;
	load(other._module.getOwner());
	return *this;
}

const vk::ShaderModule& shader::get_module() const noexcept
{
	return _module.get();
}

void shader::reload()
{
	load(_module.getOwner());
}

void shader::load(vk::Device dev)
{
	const std::filesystem::path combined = _path.string();
	std::filesystem::path final_path;

	bool path_valid = _path.is_absolute();
	if (!path_valid)
		for (auto&& inc : gfx::shader_includes::directories) {
			if (exists(inc / combined) && !path_valid) {
				final_path = inc / combined;
				path_valid = true;
			}
			if (path_valid) break;
		}

	if (!path_valid) {
		elog << "Shader not found: " << _path;
		return;
	}

	std::ifstream in(final_path, std::ios::binary | std::ios::in);
	if (in) {
		in.seekg(0, std::ios::end);
		const auto size = in.tellg();
		in.seekg(0, std::ios::beg);
		std::vector<std::byte> data(size);    // source string or binary; empty on failure
		data.resize(size);
		in.read(reinterpret_cast<char*>(data.data()), size);

		vk::ShaderModuleCreateInfo create;
		create.codeSize = size;
		create.pCode = reinterpret_cast<const u32*>(data.data());
		_module = dev.createShaderModuleUnique(create);
	}
}
}    // namespace v1
}    // namespace gfx