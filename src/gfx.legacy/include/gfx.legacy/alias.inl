#pragma once

namespace gfx
{
	template<typename T>
	using hbuffer = host_buffer<T>;
	template<typename T>
	using buffer = device_buffer<T>;
	using himage = host_image;
	using image = device_image;
}