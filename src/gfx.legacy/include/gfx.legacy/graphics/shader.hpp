#pragma once

#include <filesystem>
#include <gsl/span>

namespace gfx {
	inline namespace v1 {
		enum class shader_type
		{
			vert,
			frag,
			geom,
			tese,
			tesc,
			comp
		};

		enum class shader_format
		{
			spirv,
			text
		};

		class shader
		{
		public:
			shader(shader_type stage, const std::filesystem::path& path);
			shader(shader_type stage, const gsl::span<const uint32_t>& spirv);

			const std::vector<std::byte>& data() const noexcept;
			shader_format                 format() const noexcept;
			shader_type stage() const noexcept { return _stage; }

		private:
			shader_type _stage;
			shader_format          _format;
			std::filesystem::path  _path;
			std::vector<std::byte> _data{};    // source string or binary; empty on failure
		};
	}
}