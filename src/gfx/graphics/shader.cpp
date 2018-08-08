#include "shader.hpp"
#include <gfx/context.hpp>
#include <gfx/graphics/shader_include.hpp>
#include <glsp/glsp.hpp>

namespace gfx
{
    inline namespace v1
    {
		shader::shader(shader_type stage, const std::filesystem::path& path) : _stage(stage), _path(path)
		{
			std::string gapi_str;
			bool enable_text = false;
			switch (gfx::context::current()->options().graphics_api)
			{
			case gfx::gapi::opengl: gapi_str = "opengl"; enable_text = true; break;
			case gfx::gapi::vulkan: gapi_str = "vulkan"; break;
			}
			std::filesystem::path combined = (path.string() + ".spv-" + gapi_str);

			bool path_valid = path.is_absolute();
			if (!path_valid)
				for (auto&& inc : gfx::shader_includes::directories)
					if (exists(inc / combined))
					{
						_format = gfx::shader_format::spirv;
						_path      = inc / combined;
						path_valid = true;
						break;
					}
					else if (enable_text && exists(inc / path)) {
						_format = gfx::shader_format::text;
						_path      = inc / _path;
						path_valid = true;
						break;
					}

					if (!path_valid) {
						elog << "Shader not found: " << path;
						return;
					}

					switch (_format)
					{
					case shader_format::text:
					{
						std::vector<glsp::files::path> includes;
						for (const auto& p : shader_includes::directories) includes.push_back(p.string());
						const auto ppf = glsp::preprocess_file(_path.string(), includes);
						_data.resize(ppf.contents.size() + 1);
						memcpy(_data.data(), ppf.contents.data(), ppf.contents.size() * sizeof(char));
						_data.back() = std::byte(0);
					}
					break;
					case shader_format::spirv:
					{
						std::ifstream in(_path, std::ios::binary | std::ios::in);
						if (in) {
							in.seekg(0, std::ios::end);
							auto size = in.tellg();
							in.seekg(0, std::ios::beg);
							_data.resize(size);
							in.read(reinterpret_cast<char*>(_data.data()), size);
						}
					}
					break;
					}
		}

		const std::vector<std::byte>& shader::data() const noexcept
		{
			return _data;
		}

		shader_format shader::format() const noexcept
		{
			return _format;
		}
    }
}