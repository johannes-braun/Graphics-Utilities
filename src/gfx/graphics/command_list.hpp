#pragma once

#include "../math/geometry.hpp"
#include "binding_set.hpp"
#include "fence.hpp"
#include "framebuffer.hpp"
#include "pipeline.hpp"
#include <optional>
#include <utility>

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

struct drawcmd_indexed
{
	drawcmd_indexed() = default;
	drawcmd_indexed(u32 index_count, u32 instance_count = 1, u32 base_index = 0, u32 base_vertex = 0, u32 base_instance = 0)
		: index_count(index_count), instance_count(instance_count), base_index(base_index), base_vertex(base_vertex), base_instance(base_instance) {}

	u32 index_count = 0;
	u32 instance_count = 0;
	u32 base_index = 0;
	u32 base_vertex = 0;
	u32 base_instance = 0;
};

struct drawcmd
{
	drawcmd() = default;
	drawcmd(u32 vertex_count, u32 instance_count = 1, u32 base_vertex = 0, u32 base_instance = 0)
		: vertex_count(vertex_count), instance_count(instance_count), base_vertex(base_vertex), base_instance(base_instance) {}

	u32 vertex_count = 0;
	u32 instance_count = 0;
	u32 base_vertex = 0;
	u32 base_instance = 0;
};

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
	virtual void draw_indirect(const handle& buffer, u32 count, u32 stride, u32 first, bool indexed) = 0;

    virtual void push_binding(u32 set, u32 binding, u32 arr_element, binding_type type, std::any obj, u32 offset, u32 size) = 0;
	virtual void update_buffer(const handle& buffer, u32 offset, u32 size, const void* data) = 0;
	virtual void copy_buffer(const handle& dest, u32 dest_offset, const handle& src, u32 src_offset, u32 size) = 0;

    virtual void set_viewports(u32 first, span<viewport> vp, span<rect2f> scissors) = 0;

	virtual handle api_handle() { return {}; }
};
}    // namespace detail

class commands : public impl::implements<detail::commands_implementation>
{
public:
    commands(commands_type type);

    void reset() const;
    void begin() const;
    void end() const;

    void execute() const;
    void execute(fence& f) const;
    void execute_sync_after(const commands& cmd) const;
    void execute_sync_after(const commands& cmd, fence& f) const;

    void bind_pipeline(const compute_pipeline& p, std::initializer_list<binding_set*> bindings = {}) const;
    void dispatch(u32 x, u32 y = 1, u32 z = 1) const;

    void begin_pass(const framebuffer& fbo, std::optional<rect2f> render_area = {}) const;
    void end_pass() const;

    void bind_pipeline(const graphics_pipeline& p, std::initializer_list<binding_set*> bindings = {}) const;
	void set_viewports(u32 first, span<viewport> vp, span<rect2f> scissors) const;

	template<template<typename> typename Buffer, typename T, typename = std::enable_if_t<is_buffer<Buffer<T>>>>
    void bind_vertex_buffer(const Buffer<T>& buffer, u32 binding, i64 offset = 0) const;
	template<template<typename> typename Buffer, typename T, typename = std::enable_if_t<is_buffer<Buffer<T>>>>
    void bind_index_buffer(const Buffer<T>& buffer, index_type index, i64 offset = 0) const;

    void draw(const drawcmd& cmd) const;
    void draw_indexed(const drawcmd_indexed& cmd) const;
    template<template<typename> typename Buffer, typename T, typename = std::enable_if_t<is_buffer<Buffer<T>>>>
    void draw_indirect(const Buffer<T>& buffer, u32 count = ~0, u32 stride = sizeof(T), u32 first = 0) const;
	template<template<typename> typename Buffer, typename T, typename = std::enable_if_t<is_buffer<Buffer<T>>>>
    void draw_indirect_indexed(const Buffer<T>& buffer, u32 count = ~0, u32 stride = sizeof(T), u32 first = 0) const;

	template<template<typename> typename Buffer, typename T, typename = std::enable_if_t<is_buffer<Buffer<T>>>>
    void push_binding(u32 set, u32 binding, binding_type type, const Buffer<T>& buffer, u32 count = ~0, u32 first = 0) const;
    void push_binding(u32 set, u32 binding, binding_type type, const image_view& view, const sampler& sampler) const;
    void push_binding(u32 set, u32 binding, binding_type type, const image_view& view) const;
	template<template<typename> typename Buffer, typename T, typename = std::enable_if_t<is_buffer<Buffer<T>>>>
    void push_binding(u32 set, u32 binding, u32 array_element, binding_type type, const Buffer<T>& buffer, u32 count = ~0, u32 first = 0) const;
    void push_binding(u32 set, u32 binding, u32 array_element, binding_type type, const image_view& view, const sampler& sampler) const;
    void push_binding(u32 set, u32 binding, u32 array_element, binding_type type, const image_view& view) const;

	template<template<typename> typename Buffer, typename T, typename = std::enable_if_t<is_buffer<Buffer<T>>>>
    void update_buffer(const Buffer<T>& buffer, u32 offset, std::initializer_list<T> data) const;
	template<template<typename> typename Buffer, typename T, typename = std::enable_if_t<is_buffer<Buffer<T>>>>
    void update_buffer(const Buffer<T>& buffer, u32 offset, u32 count, const T* data) const;
	template<template<typename> typename Buffer, typename T, typename = std::enable_if_t<is_buffer<Buffer<T>>>>
    void update_buffer(const Buffer<T>& buffer, u32 offset, const T& data) const;

	template<template<typename> typename BufferA, template<typename> typename BufferB, typename T, typename = std::enable_if_t<is_buffer<BufferA<T>> && is_buffer<BufferB<T>>>>
	void copy_buffer(const BufferA<T>& dest, u32 dest_offset, const BufferB<T>& src, u32 src_offset, u32 count) const;
	template<template<typename> typename BufferA, template<typename> typename BufferB, typename T, typename = std::enable_if_t<is_buffer<BufferA<T>> && is_buffer<BufferB<T>>>>
	void copy_buffer(const BufferA<T>& dest, const BufferB<T>& src, u32 count) const;

	template<typename T, typename = decltype(std::declval<T>().render(std::declval<commands&>()))>
	auto render(T& obj);
};
}    // namespace v1
}    // namespace gfx

#include "command_list.inl"