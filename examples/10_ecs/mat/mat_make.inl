#pragma once

namespace gfx
{
    inline namespace v1
    {
		template<typename T, size_t Maj, size_t Min>
		constexpr mat<T, Maj, Min> make_diag(T value) noexcept
		{
			return mat<T, Maj, Min>(value, diagonal);
		}
		template<typename T, size_t Maj, size_t Min>
		constexpr mat<T, Maj, Min> make_identity() noexcept
		{
			return mat<T, Maj, Min>(identity);
		}
    }
}