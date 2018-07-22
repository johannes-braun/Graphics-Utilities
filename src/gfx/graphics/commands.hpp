#pragma once

#include "descriptor.hpp"
#include "framebuffer.hpp"
#include "implementation.hpp"
#include "pipeline.hpp"
#include "fence.hpp"

namespace gfx {
inline namespace v1 {
namespace detail {
class commands_implementation
{
public:
    static std::unique_ptr<commands_implementation> make();

    virtual ~commands_implementation()                                                                              = default;
    virtual void bind_graphics_pipeline(graphics_pipeline& pipeline)                                                = 0;
	virtual void bind_compute_pipeline(compute_pipeline& pipeline)                                                = 0;
    virtual void bind_vertex_buffer(uint32_t binding, std::any buffer_handle, ptrdiff_t offset, uint32_t stride)    = 0;
    virtual void bind_index_buffer(std::any buffer_handle, index_type type, ptrdiff_t offset)                       = 0;
    virtual void draw(size_t vertex_count, size_t instance_count, ptrdiff_t first_vertex, ptrdiff_t first_instance) = 0;
    virtual void draw_indexed(size_t index_count, size_t instance_count, ptrdiff_t first_index, ptrdiff_t first_vertex,
                              ptrdiff_t first_instance)                                                             = 0;
    virtual void reset()                                                                                            = 0;
    virtual void execute(fence* f)                                                                                = 0;
	virtual void dispatch_compute(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z) = 0;

    // TODO(s):
    virtual void bind_descriptors(descriptor_set* sets, int count)       = 0;
    virtual void begin_pass(framebuffer& fbo_handle)                     = 0;
    virtual void end_pass()                                              = 0;
    virtual void set_viewports(gfx::viewport* vps, int count, int first) = 0;

    virtual std::any api_handle() = 0;
};

template<typename T>
struct is_buffer : std::false_type
{
};

template<typename T>
struct is_buffer<host_buffer<T>> : std::true_type
{
};

template<typename T>
struct is_buffer<device_buffer<T>> : std::true_type
{
};

template<typename T>
using enable_if_buffer_t = std::enable_if_t<is_buffer<std::decay_t<T>>::value>;
}    // namespace detail

class commands : public impl::implements<detail::commands_implementation>
{
public:
    template<typename Buffer, typename = detail::enable_if_buffer_t<Buffer>>
    void bind_vertex_buffer(uint32_t binding, Buffer& buffer, ptrdiff_t offset = 0, uint32_t stride = sizeof(typename Buffer::value_type));

    template<typename Buffer, typename = detail::enable_if_buffer_t<Buffer>>
    void bind_index_buffer(Buffer& buffer, index_type type, ptrdiff_t offset = 0);

    void bind_pipeline(graphics_pipeline& ppl) const;
    void bind_pipeline(compute_pipeline& ppl) const;
    void draw(size_t vertex_count, size_t instance_count = 1, ptrdiff_t first_vertex = 0, ptrdiff_t first_instance = 0) const;
    void draw_indexed(size_t index_count, size_t instance_count = 1, ptrdiff_t first_index = 0, ptrdiff_t first_vertex = 0,
                      ptrdiff_t first_instance = 0) const;

	void dispatch_compute(uint32_t groups_x, uint32_t groups_y = 1, uint32_t groups_z = 1) const;

    void reset() const;
    void execute() const;
	void execute(fence& f) const;

    // TODO:
    void set_viewports(gfx::viewport* vps, int count, int first) const;
    void bind_descriptors(descriptor_set* sets, int count) const;    // etc...
    void begin_pass(framebuffer& fbo) const;
    void end_pass() const;
};

template<typename Buffer, typename>
void commands::bind_vertex_buffer(uint32_t binding, Buffer& buffer, ptrdiff_t offset, uint32_t stride)
{
    implementation()->bind_vertex_buffer(binding, buffer.api_handle(), offset, stride);
}

template<typename Buffer, typename>
void commands::bind_index_buffer(Buffer& buffer, index_type type, ptrdiff_t offset)
{
    implementation()->bind_index_buffer(buffer.api_handle(), type, offset);
}
}    // namespace v1
}    // namespace gfx
