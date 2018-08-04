#pragma once

#include "../pipeline.hpp"
#include <vulkan/vulkan.h>
#include "../general/handle.hpp"

namespace gfx
{
	inline namespace v1
	{
		namespace vulkan
		{
			class graphics_pipeline_implementation : public detail::graphics_pipeline_implementation
			{
			public:
				~graphics_pipeline_implementation() override;
				bool bind(vertex_input& input, state_info& info, const std::vector<std::pair<shader_type, std::shared_ptr<shader>>>& shaders,
					bool shaders_valid) override;

				std::any api_handle() override;

			private:
				VkDevice _device = nullptr;
				movable_handle<VkPipeline> _pipeline;
			};

			class compute_pipeline_implementation : public detail::compute_pipeline_implementation
			{
			public:
				~compute_pipeline_implementation() override;
				bool build(shader& shaders) override;

				std::any api_handle() override;

			private:
				VkDevice _device = nullptr;
				movable_handle<VkPipeline> _pipeline;
			};
		}
	}
}
