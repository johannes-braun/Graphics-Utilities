#pragma once

#include <vulkan/vulkan.h>

namespace gfx {
inline namespace v1 {
namespace vulkan {

    template<typename T>
    struct init : public T
    {
        init() : T{0} {}
		init(VkStructureType s_type) : init() { T::sType = s_type; }
    };

}
}    // namespace v1
}    // namespace gfx
