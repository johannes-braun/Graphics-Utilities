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

			shader(const shader& other);
			shader& operator=(const shader& other);
			shader(shader&& other) = default;
			shader& operator=(shader&& other) = default;

			const std::vector<std::byte>& data() const noexcept;
			const vk::ShaderModule& get_module() const noexcept;
			void reload();

		private:
			void load(vk::Device dev);

			std::filesystem::path _path;
			vk::UniqueShaderModule _module;
		};
	}
}