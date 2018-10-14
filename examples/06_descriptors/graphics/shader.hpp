#pragma once

#include <filesystem>
#include <optional>
#include <vulkan/vulkan.hpp>

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

		class device;
		class shader
		{
		public:
			explicit shader(device& dev, const std::filesystem::path& path);

			const std::vector<std::byte>& data() const noexcept;
			const vk::ShaderModule& mod() const noexcept;

		private:
			vk::UniqueShaderModule _module;
		};
	}
}