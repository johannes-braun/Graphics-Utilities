#pragma once

namespace gfx
{
    constexpr vertex3d::vertex3d(const glm::vec3 position) noexcept : position(position) { }
    constexpr vertex3d::vertex3d(const glm::vec3 position, const glm::vec3 norm) noexcept : position(position), normal(norm) { }
    constexpr vertex3d::vertex3d(const glm::vec3 position, const glm::vec2 uv, const glm::vec3 norm) noexcept : position(position), normal(norm), uv(uv) { }

    constexpr vertex2d::vertex2d(const glm::vec2 position) noexcept : position(position) {}
    constexpr vertex2d::vertex2d(const glm::vec2 position, const glm::vec2 uv) noexcept : position(position), uv(uv) {}
    constexpr vertex2d::vertex2d(const glm::vec2 position, const glm::vec2 uv, const glm::u8vec4 color) noexcept : position(position), uv(uv), color(color) { }
}