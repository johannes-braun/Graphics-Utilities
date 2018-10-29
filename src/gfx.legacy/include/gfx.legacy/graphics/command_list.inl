#pragma once

namespace gfx {
namespace v1 {
	template<template <typename> class Buffer, typename T, typename>
	void commands::bind_vertex_buffer(const Buffer<T>& buffer, u32 binding, i64 offset) const
	{
		implementation()->bind_vertex_buffer(buffer.api_handle(), binding, offset);
	}

	template<template <typename> class Buffer, typename T, typename>
	void commands::bind_index_buffer(const Buffer<T>& buffer, index_type index, i64 offset) const
	{
		implementation()->bind_index_buffer(buffer.api_handle(), index, offset);
	}

	template<template <typename> class Buffer, typename T, typename>
	void commands::draw_indirect(const Buffer<T>& buffer, u32 count, u32 stride, u32 first) const
	{
		static_assert(sizeof(T) > sizeof(drawcmd), "Buffer value type must contain the necessary indirect draw data.");
		assert(stride > sizeof(drawcmd));
		count = count == ~0 ? buffer.size() : count;
		implementation()->draw_indirect(buffer.api_handle(), count, stride, first, false);
	}

	template<template <typename> class Buffer, typename T, typename>
	void commands::draw_indirect_indexed(const Buffer<T>& buffer, u32 count, u32 stride, u32 first) const
	{
		static_assert(sizeof(T) > sizeof(drawcmd), "Buffer value type must contain the necessary indirect draw data.");
		assert(stride > sizeof(drawcmd));
		count = count == ~0 ? static_cast<u32>(buffer.size()) : count;
		implementation()->draw_indirect(buffer.api_handle(), count, stride, first, true);
	}

	template<template <typename> class Buffer, typename T, typename>
	void commands::push_binding(u32 set, u32 binding, binding_type type, const Buffer<T>& buffer, u32 count, u32 first) const
	{
		push_binding(set, binding, 0, type, buffer, count, first);
	}

	template<template <typename> class Buffer, typename T, typename>
	void commands::push_binding(u32 set, u32 binding, u32 array_element, binding_type type, const Buffer<T>& buffer, u32 count, u32 first) const
	{
		implementation()->push_binding(set, binding, array_element, type, &buffer.implementation(), first * sizeof(T),
			count == ~0 ? buffer.size() * sizeof(T) : count * sizeof(T));
	}

	template<typename T, typename>
	auto commands::render(T& obj)
	{
		return obj.render(*this);
	}

	template<template <typename> class Buffer, typename T, typename>
	void commands::update_buffer(const Buffer<T>& buffer, u32 offset, std::initializer_list<T> data) const
	{
		update_buffer(buffer, offset, std::size(data), std::data(data));
	}

	template<template <typename> class Buffer, typename T, typename>
	void commands::update_buffer(const Buffer<T>& buffer, u32 offset, u32 count, const T* data) const
	{
		implementation()->update_buffer(buffer.api_handle(), offset * sizeof(T), count * sizeof(T), data);
	}

	template<template <typename> class Buffer, typename T, typename>
	void commands::update_buffer(const Buffer<T>& buffer, u32 offset, const T& data) const
	{
		update_buffer(buffer, offset, 1, &data);
	}
	
	template<template<typename> typename BufferA, template<typename> typename BufferB, typename T, typename>
	void commands::copy_buffer(const BufferA<T>& dest, u32 dest_offset, const BufferB<T>& src, u32 src_offset, u32 count) const
	{
		implementation()->copy_buffer(dest.api_handle(), dest_offset * sizeof(T), src.api_handle(), src_offset * sizeof(T), count * sizeof(T));
	}

	template<template<typename> typename BufferA, template<typename> typename BufferB, typename T, typename>
	void commands::copy_buffer(const BufferA<T>& dest, const BufferB<T>& src, u32 count) const
	{
		copy_buffer(dest, 0, src, 0, count);
	}
}
}    // namespace gfx