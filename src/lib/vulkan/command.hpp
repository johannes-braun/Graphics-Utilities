#pragma once

#include <vulkan/vulkan.hpp>

namespace vkn
{
	namespace command
	{
		void transform_image_layout(
			vk::CommandBuffer buffer,
			vk::Image image,
			vk::ImageSubresourceRange range,
			vk::AccessFlags source_access, vk::AccessFlags destination_access,
			vk::ImageLayout source_layout, vk::ImageLayout destination_layout,
			vk::PipelineStageFlags source_stage, vk::PipelineStageFlags destination_stage);
	}
}
