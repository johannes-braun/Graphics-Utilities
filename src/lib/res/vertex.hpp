#pragma once

#include <glm/glm.hpp>

namespace res
{
    struct vertex
    {
        vertex()
            : position(0, 0, 0),
            normal{ 0, 1, 0 },
            uv{ 0, 0 }
        {}

        vertex(const glm::vec3 position, const glm::vec2 uv, const glm::vec3 norm)
            : position(position), normal(normalize(norm)), uv(uv)
        {}

        glm::vec3 position;
        uint32_t meta = 0;
        glm::vec3 normal;
        uint32_t meta2 = 0;
        glm::vec2 uv;
        uint64_t meta3 = 0;
    };
}