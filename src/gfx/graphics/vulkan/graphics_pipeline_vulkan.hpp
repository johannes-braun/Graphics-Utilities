#pragma once

#include "../graphics_pipeline.hpp"
#include <vulkan/vulkan.h>
#include "../general/handle.hpp"

namespace gfx
{
    namespace v2
    {
        namespace vulkan
        {
			class graphics_pipeline_implementation : public detail::graphics_pipeline_implementation
			{
			public:
				~graphics_pipeline_implementation();

			    void initialize(const pipeline_state& state, const renderpass_layout& renderpass, span<const v1::shader* const> shaders) override;
			    handle api_handle() override;
                
			private:
				VkDevice _device;
				movable_handle<VkPipelineLayout> _layout;
				movable_handle<VkPipeline> _pipeline;
			};

			class compute_pipeline_implementation : public detail::compute_pipeline_implementation
			{
			public:
				~compute_pipeline_implementation();
				void initialize(const pipeline_state::layout& layout, const v1::shader& cs) override;
				handle api_handle() override;

			private:
				VkDevice _device;
				movable_handle<VkPipelineLayout> _layout;
				movable_handle<VkPipeline> _pipeline;
			};
        }
    }
}