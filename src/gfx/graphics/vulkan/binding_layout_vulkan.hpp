#pragma once

#include "../binding_layout.hpp"
#include "../general/handle.hpp"
#include <vulkan/vulkan.h>
#include "init_struct.hpp"
#include <vector>

namespace gfx
{
    inline namespace v1
    {
        namespace vulkan
        {
			class binding_layout_implementation : public detail::binding_layout_implementation
			{
			public:
				~binding_layout_implementation();

				void push(binding_type type, u32 array_size) override;
			    handle api_handle() override;

			private:
				bool _invalidated = false;
				VkDevice _device = nullptr;
				movable_handle<VkDescriptorSetLayout> _layout;
				init<VkDescriptorSetLayoutCreateInfo> _create_info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
				std::vector<VkDescriptorSetLayoutBinding> _bindings;
			};

        }
    }
}
