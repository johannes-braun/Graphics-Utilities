#pragma once
#include <execution>
#include <gfx/geometry.hpp>
#include <memory>
#include <numeric>
#include <opengl/opengl.hpp>
#include <optional>
#include <stack>

namespace gfx
{
struct camera;

struct indirect_elements
{
    uint32_t count          = 0;
    uint32_t instance_count = 0;
    uint32_t base_index     = 0;
    uint32_t base_vertex    = 0;
    uint32_t base_instance  = 0;
    uint32_t vertex_count   = 0;
};
struct indirect_arrays
{
    uint32_t count          = 0;
    uint32_t instance_count = 0;
    uint32_t base_vertex    = 0;
    uint32_t base_instance  = 0;
};

struct mesh_instance
{
    alignas(16) union {
        indirect_elements elements;
        indirect_arrays   arrays;
    } indirect{};
    alignas(4) uint32_t instance_index = 0;

    alignas(16) bounds3f bounds;

    alignas(16) glm::mat4 model_matrix = glm::mat4(1.f);
    alignas(16) glm::vec3 color        = {1.f, 1.f, 1.f};
    alignas(4) float roughness         = 0.2f;
};

template <typename Indirect, typename IndirectCheck = std::enable_if_t<std::is_same_v<Indirect, indirect_elements> ||
                                                                       std::is_same_v<Indirect, indirect_arrays>>>
struct mesh_holder
{
    mesh_holder();
    mesh_instance create_mesh(const std::vector<gfx::vertex3d>& vertices, const std::vector<gfx::index32>& indices = {});
    void          update_mesh(const mesh_instance& mesh);
    void          free_mesh(const mesh_instance& mesh);
    mesh_instance copy_instance(const mesh_instance& instance);
    void          free_instance(const mesh_instance& instance);
    void          cull() const;
    void          render() const;

    gl::buffer<vertex3d>      vertex_buffer;
    gl::buffer<index32>       index_buffer;
    gl::buffer<mesh_instance> info_buffer;

private:
    gl::vertex_array     _vertex_array;
    std::stack<uint32_t> _free_meshes;

    struct range
    {
        uint32_t begin;
        uint32_t end;
    };
    std::vector<range> _free_index_ranges;
    std::vector<range> _free_vertex_ranges;
};
}

#include "mesh.inl"