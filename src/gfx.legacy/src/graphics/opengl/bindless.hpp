#pragma once

#include <gfx/type.hpp>
#include <gfx/graphics/image_view.hpp>
#include <gfx/graphics/device_buffer.hpp>
#include <mygl/mygl.hpp>
#include "formats.hpp"
#include <gfx/context.hpp>

namespace gfx
{
    inline namespace v1
    {
		namespace ext
		{
			struct buffer_ptr
			{
				buffer_ptr() = default;
				buffer_ptr(nullptr_t) {}

				template<template<typename> typename Buffer, typename T, typename = std::enable_if_t<is_buffer<Buffer<T>>>>
				buffer_ptr(const Buffer<T>& buffer)
				{
					assert(context::current()->options().graphics_api == gapi::opengl);
					const auto b = handle_cast<mygl::buffer>(buffer);
					glMakeNamedBufferResidentNV(b, GL_READ_WRITE);
					glGetNamedBufferParameteruiv64NV(b, GL_BUFFER_GPU_ADDRESS_NV, &_hnd);
					_buffer_mapping[_hnd] = b;
				}
				~buffer_ptr()
				{
					if (_hnd != 0)
					{
						const auto it = _buffer_mapping.find(_hnd);
						if (glIsBuffer(it->second) && glIsNamedBufferResidentNV(it->second))
							glMakeNamedBufferNonResidentNV(it->second);
						_buffer_mapping.erase(_hnd);
					}
				}

				explicit operator u64() const noexcept { return _hnd; }

			private:
				static inline std::unordered_map<u64, mygl::buffer> _buffer_mapping;
				alignas(8) u64 _hnd = 0;
			};

			struct sampled_image_ptr
			{
				sampled_image_ptr() = default;
				sampled_image_ptr(nullptr_t) {}

				sampled_image_ptr(const image_view& view, const sampler& smp)
				{
					assert(context::current()->options().graphics_api == gapi::opengl);
					const auto v = handle_cast<mygl::texture>(view);
					const auto s = handle_cast<mygl::sampler>(smp);
					_hnd = glGetTextureSamplerHandleARB(v, s);
					glMakeTextureHandleResidentARB(_hnd);
				}
				~sampled_image_ptr()
				{
					if (_hnd != 0)
					{
						if (glIsTextureHandleResidentARB(_hnd))
							glMakeTextureHandleNonResidentARB(_hnd);
					}
				}
				explicit operator u64() const noexcept { return _hnd; }
			private:
				alignas(8) u64 _hnd = 0;
			};

			struct storage_image_ptr
			{
				storage_image_ptr() = default;
				storage_image_ptr(nullptr_t) {}

				storage_image_ptr(const image_view& view)
				{
					assert(context::current()->options().graphics_api == gapi::opengl);
					const auto v = handle_cast<mygl::texture>(view);
					_hnd = glGetImageHandleARB(v, view.base_mip(), true, view.base_layer(), opengl::format_from(view.pixel_format()).internal_format);
					glMakeImageHandleResidentARB(_hnd, GL_READ_WRITE);
				}
				~storage_image_ptr()
				{
					if (_hnd != 0)
					{
						if (glIsImageHandleResidentARB(_hnd))
							glMakeImageHandleNonResidentARB(_hnd);
					}
				}
				explicit operator u64() const noexcept { return _hnd; }
			private:
				alignas(16) u64 _hnd = 0;
				u64 _pad = 0;
			};
		}
    }
}
