#pragma once

#include <jpu/data>
#include <glm/glm.hpp>
#include <filesystem>

namespace res
{
    struct vertex
    {
        vertex() : position(0, 0, 0, 1), uv{ 0, 0, 0, 0 },  normal{ 0, 1, 0, 0 }
        {}
        vertex(const glm::vec3 position, const glm::vec2 uv, const glm::vec3 norm)
            : position(position, 1), uv(uv, 0, 0), normal(norm, 0)
        {}

        glm::vec4 position;
        glm::vec4 uv;
        glm::vec4 normal;
    };

    struct material
    {

    };

    struct mesh
    {
        const material* material;
        std::vector<uint32_t> indices;
        std::vector<vertex> vertices;
    };

    struct geometry_file
    {
        jpu::named_vector<std::string, mesh> meshes;
        jpu::named_vector<std::string, material> materials;
        
        friend geometry_file load_geometry(const std::experimental::filesystem::path& path);
    };

    geometry_file load_geometry(const std::experimental::filesystem::path& path);
}
