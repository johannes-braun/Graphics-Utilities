#pragma once

#include <array>
#include <glm/glm.hpp>

namespace geo
{
    struct vertex
    {
        vertex()
            : position(make_vec4(0, 0, 0, 1)),
            uv{ make_vec4(0, 0, 0, 0) },
            normal{ make_vec4(0, 1, 0, 0) },
            tangent{ make_vec4(0, 0, 1, 0) },
            bitangent{1, 0, 0, 0}
        {}
        vertex(const glm::vec3 position, const glm::vec2 uv, const glm::vec3 norm, const glm::vec3 tan, const glm::vec3 bitangent)
            : position(position, 1), uv(uv, 0, 0), normal(norm, 0), tangent(tan, 0), bitangent(bitangent, 0)
        {}
        vertex(const glm::vec3 position, const glm::vec2 uv, const glm::vec3 norm, const glm::vec3 tan)
            : position(position, 1), uv(uv, 0, 0), normal(norm, 0), tangent(tan, 0), bitangent(glm::cross(tan, norm), 0)
        {}

        glm::vec4 position;
        glm::vec4 uv;
        glm::vec4 normal;
        glm::vec4 tangent;
        glm::vec4 bitangent;

    private:
        static glm::vec4 make_vec4(float x, float y, float z, float w)
        {
            return glm::vec4({x}, {y}, {z}, {w});
        }
    };

    namespace cube
	{
		const std::array<vertex, 24> vertices{
			//Back
			vertex({ -1, 1, -1 },{ 0, 1 },{ 0, 0, -1 },{ 0, -1, 0 }),
			vertex({ 1, 1, -1 },{ 1, 1 },{ 0, 0, -1 },{ 0, -1, 0 }),
			vertex({ 1, -1, -1 },{ 1, 0 },{ 0, 0, -1 },{ 0, -1, 0 }),
			vertex({ -1, -1, -1 },{ 0, 0 },{ 0, 0, -1 },{ 0, -1, 0 }),

			//Front
			vertex({ -1, 1, 1 },{ 0, 1 },{ 0, 0, 1 },{ 0, 1, 0 }),
			vertex({ 1, 1, 1 },{ 1, 1 },{ 0, 0, 1 },{ 0, 1, 0 }),
			vertex({ 1, -1, 1 },{ 1, 0 },{ 0, 0, 1 },{ 0, 1, 0 }),
			vertex({ -1, -1, 1 },{ 0, 0 },{ 0, 0, 1 },{ 0, 1, 0 }),

			//Bottom
			vertex({ -1, -1, -1 },{ 0, 0 },{ 0, -1, 0 },{ 0, 0, 1 }),
			vertex({ 1, -1, -1 },{ 0, 1 },{ 0, -1, 0 },{ 0, 0, 1 }),
			vertex({ 1, -1, 1 },{ 1, 1 },{ 0, -1, 0 },{ 0, 0, 1 }),
			vertex({ -1, -1, 1 },{ 0, 1 },{ 0, -1, 0 },{ 0, 0, 1 }),

			//Top
			vertex({ -1, 1, -1 },{ 0, 0 },{ 0, 1, 0 },{ 0, 0, -1 }),
			vertex({ 1, 1, -1 },{ 1, 0 },{ 0, 1, 0 },{ 0, 0, -1 }),
			vertex({ 1, 1, 1 },{ 1, 1 },{ 0, 1, 0 },{ 0, 0, -1 }),
			vertex({ -1, 1, 1 },{ 0, 1 },{ 0, 1, 0 },{ 0, 0, -1 }),

			//Right
			vertex({ 1, -1, -1 },{ 0, 0 },{ 1, 0, 0 },{ 0, 1, 0 }),
			vertex({ 1, 1, -1 },{ 1, 0 },{ 1, 0, 0 },{ 0, 1, 0 }),
			vertex({ 1, 1, 1 },{ 1, 1 },{ 1, 0, 0 },{ 0, 1, 0 }),
			vertex({ 1, -1, 1 },{ 0, 1 },{ 1, 0, 0 },{ 0, 1, 0 }),

			//Left
			vertex({ -1, -1, -1 },{ 0, 0 },{ -1, 0, 0 },{ 0, -1, 0 }),
			vertex({ -1, 1, -1 },{ 1, 0 },{ -1, 0, 0 },{ 0, -1, 0 }),
			vertex({ -1, 1, 1 },{ 1, 1 },{ -1, 0, 0 },{ 0, -1, 0 }),
			vertex({ -1, -1, 1 },{ 0, 1 },{ -1, 0, 0 },{ 0, -1, 0 }),
		};

		constexpr std::array<uint32_t, 36> indices{
			0, 1, 2,        0, 2, 3,
			6, 5, 4,        7, 6, 4,
			8, 9, 10,       8, 10, 11,
			14, 13, 12,     15, 14, 12,
			16, 17, 18,     16, 18, 19,
			22, 21, 20,     23, 22, 20,
		};
	}
}
