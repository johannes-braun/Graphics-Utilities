#pragma once

#include <glm/glm.hpp>

namespace res
{
    struct vertex
    {
        vertex()
            : position(0, 0, 0, 1),
            uv{ 0, 0, 0, 0 },
            normal{ 0, 1, 0, 0 }
        {}

        vertex(const glm::vec3 position, const glm::vec2 uv, const glm::vec3 norm)
            : position(position, 1), uv(uv, 0, 0), normal(norm, 0)
        {}

        glm::vec4 position;
        glm::vec4 uv;
        glm::vec4 normal;
    };
}