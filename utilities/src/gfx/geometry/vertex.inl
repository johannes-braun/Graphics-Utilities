#pragma once

namespace gfx
{
    constexpr vertex::vertex(const glm::vec3 position) noexcept : position(position) { }
    constexpr vertex::vertex(const glm::vec3 position, const glm::vec3 norm) noexcept : position(position), normal(norm) { }
    constexpr vertex::vertex(const glm::vec3 position, const glm::vec2 uv, const glm::vec3 norm) noexcept : position(position), normal(norm), uv(uv) { }
}