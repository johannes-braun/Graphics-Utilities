#pragma once

#include <gsl/span>

namespace gfx
{
    constexpr ptrdiff_t dynamic_extent = gsl::dynamic_extent;
    template<typename T, ptrdiff_t Extent = dynamic_extent>
    using span = gsl::span<T, Extent>;

	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;

	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;
}