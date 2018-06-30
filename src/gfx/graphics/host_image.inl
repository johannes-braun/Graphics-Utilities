#pragma once

namespace gfx
{
constexpr extent::extent(uint32_t width, uint32_t height, uint32_t depth)
        : width(width)
        , height(height)
        , depth(depth)
{
}
constexpr extent::extent(extent extent2d, uint32_t depth)
        : width(extent2d.width)
        , height(extent2d.height)
        , depth(depth)
{
}
constexpr extent::extent(extent extent1d, uint32_t height, uint32_t depth)
        : width(extent1d.width)
        , height(height)
        , depth(depth)
{
}

constexpr extent::operator glm::uvec3() const noexcept { return glm::uvec3(width, height, depth); }
constexpr size_t  extent::count() const noexcept { return width * height * depth; }

constexpr extent extent::as_1d() const noexcept { return {width, height, 1}; }
constexpr extent extent::as_2d() const noexcept { return {width, height, 1}; }
}