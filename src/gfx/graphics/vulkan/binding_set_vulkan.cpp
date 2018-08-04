#include "binding_layout_vulkan.hpp"
#include "binding_set_vulkan.hpp"
#include "context_vulkan.hpp"

namespace gfx {
inline namespace v1 {
namespace vulkan {

binding_set_implementation::~binding_set_implementation()
{
    if (_set) vkFreeDescriptorSets(_device, _pool, 1, &_set);
}

void binding_set_implementation::initialize(const binding_layout& layout)
{
    auto& ctx  = context::current();
    auto  impl = static_cast<context_implementation*>(std::any_cast<detail::context_implementation*>(ctx->implementation()));
    _device    = impl->device();
    _pool      = impl->descriptor_pool();

    init<VkDescriptorSetAllocateInfo> alloc{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    alloc.descriptorSetCount = 1;
    auto l                   = handle_cast<VkDescriptorSetLayout>(layout);
    alloc.pSetLayouts        = &l;
    alloc.descriptorPool     = impl->descriptor_pool();

    vkAllocateDescriptorSets(_device, &alloc, &_set);
}

handle binding_set_implementation::api_handle()
{
    return VkDescriptorSet(_set);
}
void binding_set_implementation::bind(u32 binding, u32 arr_element, binding_type type, std::any obj)
{
	if (obj.type() == typeid(std::pair<const std::unique_ptr<detail::image_view_implementation>*, const std::unique_ptr<detail::sampler_implementation>*>))
	{
		assert(type == binding_type::sampled_image);

	    auto[ivp, sp] = std::any_cast<std::pair<const std::unique_ptr<detail::image_view_implementation>*, const std::unique_ptr<detail::sampler_implementation>*>>(obj);

		auto& iv = *ivp;
		auto& s = *sp;

		auto img_view = std::any_cast<VkImageView>(iv->api_handle());
		auto smp = std::any_cast<VkSampler>(s->api_handle());

		init<VkWriteDescriptorSet> write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.dstArrayElement = arr_element;
		write.dstBinding = binding;
		write.dstSet = _set;

		init<VkDescriptorImageInfo> img_info;
		img_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		img_info.imageView = img_view;
		img_info.sampler = smp;
		write.pImageInfo = &img_info;
		vkUpdateDescriptorSets(_device, 1, &write, 0, nullptr);
	}
	else if(obj.type() == typeid(const std::unique_ptr<detail::image_view_implementation>*))
	{
		assert(type == binding_type::storage_image);
		auto& iv = *std::any_cast<const std::unique_ptr<detail::image_view_implementation>*>(obj);

		auto img_view = std::any_cast<VkImageView>(iv->api_handle());

		init<VkWriteDescriptorSet> write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		write.dstArrayElement = arr_element;
		write.dstBinding = binding;
		write.dstSet = _set;

		init<VkDescriptorImageInfo> img_info;
		img_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		img_info.imageView = img_view;
		img_info.sampler = nullptr;
		write.pImageInfo = &img_info;
		vkUpdateDescriptorSets(_device, 1, &write, 0, nullptr);
	}
	else if(obj.type() == typeid(const std::unique_ptr<detail::host_buffer_implementation>*))
	{
		auto& buf = *std::any_cast<const std::unique_ptr<detail::host_buffer_implementation>*>(obj);
		auto buffer = std::any_cast<VkBuffer>(buf->api_handle());

		init<VkWriteDescriptorSet> write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		write.descriptorCount = 1;
		write.descriptorType = [=]{
		    switch (type)
		    {
			case binding_type::storage_buffer:
				return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			case binding_type::uniform_buffer:
				return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			default: VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		    }
		}();
		write.dstArrayElement = arr_element;
		write.dstBinding = binding;
		write.dstSet = _set;

		init<VkDescriptorBufferInfo> buf_info;
		buf_info.buffer = buffer;
		buf_info.range = VK_WHOLE_SIZE;
		write.pBufferInfo = &buf_info;

		vkUpdateDescriptorSets(_device, 1, &write, 0, nullptr);
	}
	else if(obj.type() == typeid(const std::unique_ptr<detail::device_buffer_implementation>*))
	{
		auto& buf = *std::any_cast<const std::unique_ptr<detail::device_buffer_implementation>*>(obj);
		auto buffer = std::any_cast<VkBuffer>(buf->api_handle());

		init<VkWriteDescriptorSet> write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		write.descriptorCount = 1;
		write.descriptorType = [=]{
			switch (type)
			{
			case binding_type::storage_buffer:
				return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			case binding_type::uniform_buffer:
				return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			default: VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			}
		}();
		write.dstArrayElement = arr_element;
		write.dstBinding = binding;
		write.dstSet = _set;

		init<VkDescriptorBufferInfo> buf_info;
		buf_info.buffer = buffer;
		buf_info.range = VK_WHOLE_SIZE;
		write.pBufferInfo = &buf_info;

		vkUpdateDescriptorSets(_device, 1, &write, 0, nullptr);
	}
}
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx