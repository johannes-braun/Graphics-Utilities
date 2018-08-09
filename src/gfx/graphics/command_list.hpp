#pragma once

#include "../geometry.hpp"
#include "binding_set.hpp"
#include "fence.hpp"
#include "framebuffer.hpp"
#include "graphics_pipeline.hpp"
#include <optional>

namespace gfx {
inline namespace v1 {
enum class commands_type
{
    graphics,
    compute,
    transfer
};
enum class index_type
{
    uint16 = 2,
    uint32 = 4
};
class commands;

namespace detail {
class commands_implementation
{
public:
    virtual ~commands_implementation() = default;
    static std::unique_ptr<commands_implementation> make();

    virtual void initialize(commands_type type) = 0;

    virtual void reset() = 0;
    virtual void begin() = 0;
    virtual void end()   = 0;

    virtual void execute_sync_after(const commands& cmd, fence* f) = 0;
    virtual void execute(fence* f)                                 = 0;

    virtual void bind_pipeline(const compute_pipeline& p, std::initializer_list<binding_set*> bindings) = 0;
    virtual void dispatch(u32 x, u32 y, u32 z)                                                          = 0;

    virtual void begin_pass(const framebuffer& fbo, std::optional<rect2f> render_area) = 0;
    virtual void end_pass()                                                            = 0;

    virtual void bind_pipeline(const graphics_pipeline& p, std::initializer_list<binding_set*> bindings)               = 0;
    virtual void bind_vertex_buffer(const handle& buffer, u32 binding, i64 offset)                                     = 0;
    virtual void bind_index_buffer(const handle& buffer, index_type index, i64 offset)                                 = 0;
    virtual void draw(u32 vertex_count, u32 instance_count, u32 base_vertex, u32 base_instance)                        = 0;
    virtual void draw_indexed(u32 index_count, u32 instance_count, u32 base_index, u32 base_vertex, u32 base_instance) = 0;

    virtual void push_binding(u32 set, u32 binding, u32 arr_element, binding_type type, std::any obj) = 0;

    virtual void set_viewports(u32 first, span<viewport> vp, span<rect2f> scissors) = 0;
};
}    // namespace detail

class commands : public impl::implements<detail::commands_implementation>
{
public:
    commands(commands_type type) { implementation()->initialize(type); }

    void reset() { implementation()->reset(); }
    void begin() { implementation()->begin(); }
    void end() { implementation()->end(); }

    void execute_sync_after(const commands& cmd, fence* f = nullptr) { implementation()->execute_sync_after(cmd, f); }
    void execute(fence* f = nullptr) { implementation()->execute(f); }

    void bind_pipeline(const compute_pipeline& p, std::initializer_list<binding_set*> bindings = {})
    {
        implementation()->bind_pipeline(p, bindings);
    }
    void dispatch(u32 x, u32 y = 1, u32 z = 1) { implementation()->dispatch(x, y, z); }

    void begin_pass(const framebuffer& fbo, std::optional<rect2f> render_area = {}) { implementation()->begin_pass(fbo, render_area); }
    void end_pass() { implementation()->end_pass(); }

    void bind_pipeline(const graphics_pipeline& p, std::initializer_list<binding_set*> bindings = {})
    {
        implementation()->bind_pipeline(p, bindings);
    }
    template<typename T>
    void bind_vertex_buffer(const device_buffer<T>& buffer, u32 binding, i64 offset = 0)
    {
        implementation()->bind_vertex_buffer(buffer.api_handle(), binding, offset);
    }
    template<typename T>
    void bind_index_buffer(const device_buffer<T>& buffer, index_type index, i64 offset = 0)
    {
        implementation()->bind_index_buffer(buffer.api_handle(), index, offset);
    }
    template<typename T>
    void bind_vertex_buffer(const host_buffer<T>& buffer, u32 binding, i64 offset = 0)
    {
        implementation()->bind_vertex_buffer(buffer.api_handle(), binding, offset);
    }
    template<typename T>
    void bind_index_buffer(const host_buffer<T>& buffer, index_type index, i64 offset = 0)
    {
        implementation()->bind_index_buffer(buffer.api_handle(), index, offset);
    }
    void draw(u32 vertex_count, u32 instance_count = 1, u32 base_vertex = 0, u32 base_instance = 0)
    {
        implementation()->draw(vertex_count, instance_count, base_vertex, base_instance);
    }
    void draw_indexed(u32 index_count, u32 instance_count = 1, u32 base_index = 0, u32 base_vertex = 0, u32 base_instance = 0)
    {
        implementation()->draw_indexed(index_count, instance_count, base_index, base_vertex, base_instance);
    }

    template<typename T>
    void push_binding(u32 set, u32 binding, binding_type type, const device_buffer<T>& buffer)
    {
        push_binding(set, binding, 0, type, buffer);
    }
    template<typename T>
    void push_binding(u32 set, u32 binding, binding_type type, const host_buffer<T>& buffer)
    {
        push_binding(set, binding, 0, type, buffer);
    }
    void push_binding(u32 set, u32 binding, binding_type type, const image_view& view, const sampler& sampler)
    {
        push_binding(set, binding, 0, type, view, sampler);
    }
    void push_binding(u32 set, u32 binding, binding_type type, const image_view& view) { push_binding(set, binding, 0, type, view); }
    template<typename T>
    void push_binding(u32 set, u32 binding, u32 array_element, binding_type type, const device_buffer<T>& buffer)
    {
        implementation()->push_binding(set, binding, array_element, type, &buffer.implementation());
    }
    template<typename T>
    void push_binding(u32 set, u32 binding, u32 array_element, binding_type type, const host_buffer<T>& buffer)
    {
        implementation()->push_binding(set, binding, array_element, type, &buffer.implementation());
    }
    void push_binding(u32 set, u32 binding, u32 array_element, binding_type type, const image_view& view, const sampler& sampler)
    {
        std::pair<const std::unique_ptr<detail::image_view_implementation>*, const std::unique_ptr<detail::sampler_implementation>*> p(
            &view.implementation(), &sampler.implementation());
        implementation()->push_binding(set, binding, array_element, type, p);
    }
    void push_binding(u32 set, u32 binding, u32 array_element, binding_type type, const image_view& view)
    {
        implementation()->push_binding(set, binding, array_element, type, &view.implementation());
    }
    void set_viewports(u32 first, span<viewport> vp, span<rect2f> scissors) { implementation()->set_viewports(first, vp, scissors); }
};

}    // namespace v1
}    // namespace gfx
