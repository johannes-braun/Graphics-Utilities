#pragma once

#include <jpu/data>
#include "vertex.hpp"
#include "transform.hpp"
#include <filesystem>

namespace res
{
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
