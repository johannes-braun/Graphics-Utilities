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
				~graphics_pipeline_implementation();

			    void initialize(const pipe_state& state, const renderpass_layout& renderpass, span<const v1::shader* const> shaders) override;
			    handle api_handle() override;

				VkPipelineLayout layout() const noexcept { return _layout; }
                
			private:
				VkDevice _device;
				movable_handle<VkPipelineLayout> _layout;
				movable_handle<VkPipeline> _pipeline;
			};

			class compute_pipeline_implementation : public detail::compute_pipeline_implementation
			{
			public:
				~compute_pipeline_implementation();
				void initialize(const pipe_state::binding_layouts& layout, const v1::shader& cs) override;
				handle api_handle() override;
				VkPipelineLayout layout() const noexcept { return _layout; }
			private:
				VkDevice _device;
				movable_handle<VkPipelineLayout> _layout;
				movable_handle<VkPipeline> _pipeline;
			};
        }
    }
}