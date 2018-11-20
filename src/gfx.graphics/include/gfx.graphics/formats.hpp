#pragma once

namespace gfx {
inline namespace v1 {
enum format
{
    // unorm
    r8unorm,
    rg8unorm,
    rgb8unorm,
    rgba8unorm,
    r16unorm,
    rg16unorm,
    rgb16unorm,
    rgba16unorm,

    // snorm
    r8snorm,
    rg8snorm,
    rgb8snorm,
    rgba8snorm,
    r16snorm,
    rg16snorm,
    rgb16snorm,
    rgba16snorm,

    // uint
    r8u,
    rg8u,
    rgb8u,
    rgba8u,
    r16u,
    rg16u,
    rgb16u,
    rgba16u,
    r32u,
    rg32u,
    rgb32u,
    rgba32u,

    // int
    r8i,
    rg8i,
    rgb8i,
    rgba8i,
    r16i,
    rg16i,
    rgb16i,
    rgba16i,
    r32i,
    rg32i,
    rgb32i,
    rgba32i,

    // float
    r16f,
    rg16f,
    rgb16f,
    rgba16f,
    r32f,
    rg32f,
    rgb32f,
    rgba32f,

    // special formats
    rgb5a1unorm,
    rgb10a2unorm,
    rgb10a2snorm,
    r11g11b10f,
    rgb9e5,
    r5g6b5unorm,

    // depth/stencil formats
    d16unorm,
    d24unorm,
    s8ui,
    d32f,
    d24unorms8ui,
    d32fs8ui,

	// BGR/BGRA
	bgr8unorm,
	bgra8unorm,
};

[[nodiscard]] constexpr auto format_element_size(format fmt) noexcept -> size_t;
}    // namespace v1
}    // namespace gfx

#include "formats.inl"