#pragma once

#include "../command_list.hpp"
#include "context_vulkan.hpp"
#include "../general/handle.hpp"

namespace gfx {
namespace v2 {
namespace vulkan {
class commands_implementation : public detail::commands_implementation
{
public:
    // Geerbt �ber commands_implementation
    void initialize(commands_type type) override;

    void reset() override;
    void begin() override;
    void end() override;

    void execute_sync_after(const commands& cmd, fence* f) override;
    void execute(fence* f) override;

    void bind_pipeline(const compute_pipeline& p, std::initializer_list<binding_set*> bindings) override;
    void dispatch(u32 x, u32 y, u32 z) override;

    void begin_pass(const framebuffer& fbo, std::optional<rect2f> render_area) override;
    void end_pass() override;

    void bind_pipeline(const graphics_pipeline& p, std::initializer_list<binding_set*> bindings) override;
    void draw(u32 vertex_count, u32 instance_count, u32 base_vertex, u32 base_instance) override;

private:
	commands_type _type;
    VkRect2D                             _last_render_area;
    gfx::vulkan::context_implementation* _ctx_impl;
    gfx::u32                             _family;
    std::vector<VkDescriptorSet>         _sets;
    VkDevice                             _device = nullptr;
    VkCommandPool                        _pool   = nullptr;
    VkQueue                              _queue  = nullptr;
    gfx::movable_handle<VkCommandBuffer> _cmd    = nullptr;
    gfx::movable_handle<VkSemaphore>     _signal = nullptr;
};
}    // namespace vulkan
}    // namespace v2
}    // namespace gfx