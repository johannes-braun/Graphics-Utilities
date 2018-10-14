#include "shader.hpp"
#include "device.hpp"
#include <gfx/graphics/shader_include.hpp>
#include <gfx/log.hpp>
#include <fstream>

namespace gfx {
inline namespace v1 {
shader::shader(device& dev, const std::filesystem::path& path)
{
    const std::filesystem::path combined  = path.string();
	std::filesystem::path final_path;

    bool path_valid = path.is_absolute();
    if (!path_valid)
        for (auto&& inc : gfx::shader_includes::directories) {
            if (exists(inc / combined) && !path_valid) {
				final_path = inc / combined;
                path_valid = true;
            }
            if (path_valid) break;
        }

    if (!path_valid) {
        elog << "Shader not found: " << path;
        return;
    }

    std::ifstream in(final_path, std::ios::binary | std::ios::in);
    if (in) {
        in.seekg(0, std::ios::end);
        auto size = in.tellg();
        in.seekg(0, std::ios::beg);
		std::vector<std::byte> data(size);    // source string or binary; empty on failure
		data.resize(size);
        in.read(reinterpret_cast<char*>(data.data()), size);

		vk::ShaderModuleCreateInfo create;
		create.codeSize = size;
		create.pCode = reinterpret_cast<const u32*>(data.data());
		_module = dev.dev().createShaderModuleUnique(create);
    }
}
const vk::ShaderModule& shader::mod() const noexcept
{
	return _module.get();
}
}    // namespace v1
}    // namespace gfx