#pragma once

#include "../binding_set.hpp"
#include <vulkan/vulkan.h>
#include "../general/handle.hpp"
#include "init_struct.hpp"

namespace gfx {
	inline namespace v1 {
		namespace vulkan {

            class binding_set_implementation : public detail::binding_set_implementation
            {
            public:
				~binding_set_implementation();
                void initialize(const binding_layout& layout) override;
                handle api_handle() override;
				void bind(u32 binding, u32 arr_element, binding_type type, std::any obj) override;

            private:
				VkDevice _device;
				VkDescriptorPool _pool;
				movable_handle<VkDescriptorSet> _set;
            };

		}
	}    // namespace v1
}    // namespace gfx