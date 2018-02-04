#include "command.hpp"

namespace vkn
{
	void command::transform_image_layout(const vk::CommandBuffer buffer,
		const vk::Image image, vk::ImageSubresourceRange range,
		const vk::AccessFlags source_access, const vk::AccessFlags destination_access,
		const vk::ImageLayout source_layout, const vk::ImageLayout destination_layout, 
		const vk::PipelineStageFlags source_stage, const vk::PipelineStageFlags destination_stage)
	{
		vk::ImageMemoryBarrier image_memory_barrier;
		image_memory_barrier.srcAccessMask = source_access;
		image_memory_barrier.dstAccessMask = destination_access;
		image_memory_barrier.image = image;
		image_memory_barrier.oldLayout = source_layout;
		image_memory_barrier.newLayout = destination_layout;
		image_memory_barrier.subresourceRange = std::move(range);
		buffer.pipelineBarrier(source_stage, destination_stage, vk::DependencyFlags(), {}, {}, { image_memory_barrier });
	}
}
