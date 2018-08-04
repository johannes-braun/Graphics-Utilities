#pragma once

#include "../general/handle.hpp"
#include "../sampler.hpp"
#include "init_struct.hpp"
#include <vulkan/vulkan.h>

namespace gfx {
inline namespace v1 {
namespace vulkan {

class sampler_implementation : public detail::sampler_implementation
{
public:
	sampler_implementation();
    ~sampler_implementation() override;
    void     set_filter(filter_mode mode, filter filter) override;
    void     set_wrap(wrap w, wrap_mode mode) override;
    void     set_border(border_color color) override;
    void     set_lod(lod mode, float value) override;
    void     set_anisotropy(bool enable, float value) override;
    void     set_compare(bool enable, compare_op op) override;
    std::any api_handle() override;

private:
	void invalidate_handle();
	void init_handle();

    VkDevice                  _device = nullptr;
    movable_handle<VkSampler> _sampler;
    init<VkSamplerCreateInfo> _create_info { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
};

}    // namespace vulkan
}    // namespace v1
}    // namespace gfx