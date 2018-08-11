#pragma once

#include "../command_list.hpp"
#include "../general/handle.hpp"
#include "context_vulkan.hpp"

namespace gfx {
inline namespace v1 {
namespace vulkan {
class commands_implementation : public detail::commands_implementation
{
public:
    ~commands_implementation();
    // Geerbt über commands_implementation
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

    void bind_vertex_buffer(const handle& buffer, u32 binding, i64 offset) override;
    void bind_index_buffer(const handle& buffer, index_type index, i64 offset) override;
    void draw_indexed(u32 index_count, u32 instance_count, u32 base_index, u32 base_vertex, u32 base_instance) override;

    void push_binding(u32 set, u32 binding, u32 arr_element, binding_type type, std::any obj, u32 offset, u32 size) override;
    void set_viewports(u32 first, span<viewport> vp, span<rect2f> scissors);

	handle api_handle() { return VkCommandBuffer(_cmd); }

private:
	movable_handle<VkFence> _default_fence = nullptr;
    commands_type                        _type;
    VkRect2D                             _last_render_area;
    gfx::vulkan::context_implementation* _ctx_impl;
    gfx::u32                             _family;
    std::vector<VkDescriptorSet>         _sets;
    const graphics_pipeline*             _last_gpipeline = nullptr;
    const compute_pipeline*              _last_cpipeline = nullptr;
    VkDevice                             _device         = nullptr;
    VkCommandPool                        _pool           = nullptr;
    VkQueue                              _queue          = nullptr;
    gfx::movable_handle<VkCommandBuffer> _cmd            = nullptr;
    gfx::movable_handle<VkSemaphore>     _signal         = nullptr;
};
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx
