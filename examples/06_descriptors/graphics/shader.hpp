#pragma once

#include <filesystem>
#include <optional>
#include <vulkan/vulkan.hpp>
#include <gsl/span>
#include <gfx.core/types.hpp>

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
			explicit shader(device& dev, gsl::span<const u32> source);

			shader(const shader& other) = delete;
			shader& operator=(const shader& other) = delete;
			shader(shader&& other) = default;
			shader& operator=(shader&& other) = default;

			const std::vector<std::byte>& data() const noexcept;
			const vk::ShaderModule& get_module() const noexcept;

		private:
			void load(vk::Device dev, gsl::span<const u32> source);

			vk::UniqueShaderModule _module;
		};
	}
}