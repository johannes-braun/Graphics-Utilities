#pragma once

#include <gsl/span>

namespace gfx
{
    constexpr ptrdiff_t dynamic_extent = gsl::dynamic_extent;
    template<typename T, ptrdiff_t Extent = dynamic_extent>
    using span = gsl::span<T, Extent>;
}