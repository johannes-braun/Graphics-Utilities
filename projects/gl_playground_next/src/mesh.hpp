#pragma once
#include <gfx/geometry.hpp>
#include <memory>
#include <opengl/opengl.hpp>
#include <optional>

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

template <typename Indirect,
          typename = std::enable_if_t<std::is_same_v<Indirect, indirect_elements> ||
                                      std::is_same_v<Indirect, indirect_arrays>>>
struct mesh_holder
{
    mesh_holder()
            : vertex_buffer(GL_DYNAMIC_STORAGE_BIT)
            , index_buffer(GL_DYNAMIC_STORAGE_BIT)
            , info_buffer(GL_DYNAMIC_STORAGE_BIT)
    {
        _vertex_array.attrib(0)
                .enable(true)
                .format(3, GL_FLOAT, false, offsetof(gfx::vertex3d, position))
                .bind(0);
        _vertex_array.attrib(1)
                .enable(true)
                .format(2, GL_FLOAT, false, offsetof(gfx::vertex3d, uv))
                .bind(0);
        _vertex_array.attrib(2)
                .enable(true)
                .format(3, GL_FLOAT, false, offsetof(gfx::vertex3d, normal))
                .bind(0);
    }

    mesh_instance create_mesh(const std::vector<gfx::vertex3d>& vertices,
                              const std::vector<gfx::index32>&  indices = {})
    {
        mesh_instance mesh;
        if constexpr(std::is_same_v<Indirect, indirect_elements>)
        {
            mesh.indirect.elements.count          = static_cast<int>(indices.size());
            mesh.indirect.elements.instance_count = 1;
            mesh.indirect.elements.base_index     = static_cast<int>(index_buffer.size());
            mesh.indirect.elements.base_vertex    = static_cast<int>(vertex_buffer.size());
            mesh.indirect.elements.base_instance  = 0;
        }
        else if constexpr(std::is_same_v<Indirect, indirect_arrays>)
        {
            mesh.indirect.arrays.count          = static_cast<int>(vertices.size());
            mesh.indirect.arrays.instance_count = 1;
            mesh.indirect.arrays.base_vertex    = static_cast<int>(vertex_buffer.size());
            mesh.indirect.arrays.base_instance  = 0;
        }
        else
            static_assert(false, "Unsupported type");

        if(!indices.empty())
            index_buffer.insert(index_buffer.end(), indices.begin(), indices.end());
        vertex_buffer.insert(vertex_buffer.end(), vertices.begin(), vertices.end());
        _vertex_array.vertex_buffer(0, vertex_buffer);
        _vertex_array.element_buffer(index_buffer);

        mesh.instance_index = info_buffer.size();
        info_buffer.push_back(mesh);

        struct reduction
        {
            bounds3f operator()(const bounds3f& b, const vertex3d& x) const
            {
                return b + x.position;
            }
            bounds3f operator()(const vertex3d& x, const bounds3f& b) const
            {
                return b + x.position;
            }
            bounds3f operator()(const vertex3d& b, const vertex3d& x) const
            {
                bounds3f bounds;
                bounds += b.position;
                return bounds + x.position;
            }
            bounds3f operator()(const bounds3f& b, const bounds3f& x) const { return b + x; }
        };

        mesh.bounds = std::reduce(std::execution::par_unseq,
                                  vertices.begin(),
                                  vertices.end(),
                                  mesh.bounds,
                                  reduction());

        return mesh;
    }

    void update_mesh(const mesh_instance& mesh)
    {
        info_buffer[mesh.instance_index] = mesh;
        info_buffer.synchronize();
    }

    void render()
    {
        _vertex_array.bind();
        info_buffer.bind(GL_SHADER_STORAGE_BUFFER, 10);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, info_buffer);
        glMultiDrawElementsIndirect(
                GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, info_buffer.size(), sizeof(mesh_instance));
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mygl::buffer(0));
    }

    gl::buffer<vertex3d>      vertex_buffer;
    gl::buffer<index32>       index_buffer;
    gl::buffer<mesh_instance> info_buffer;
    gl::vertex_array          _vertex_array;
};

struct instance_info
{
    alignas(16) union {
        struct
        {
            uint32_t count          = 0;
            uint32_t instance_count = 0;
            uint32_t base_index     = 0;
            uint32_t base_vertex    = 0;
            uint32_t base_instance  = 0;
        } elements;

        struct
        {
            uint32_t count;
            uint32_t instance_count;
            uint32_t base_vertex;
            uint32_t base_instance;
        } arrays;
    } indirect{};

    alignas(16) glm::mat4 model_matrix = glm::mat4(1.f);
    alignas(16) glm::vec3 color        = {1.f, 1.f, 1.f};
    alignas(4) float roughness         = 0.2f;
};

struct mesh_provider
{
    mesh_provider();

    gl::buffer<vertex3d>      vertex_buffer;
    gl::buffer<index32>       index_buffer;
    gl::buffer<instance_info> info_buffer;

    static void render_all();

private:
    gl::vertex_array _vertex_array;
};

class tri_mesh : public std::enable_shared_from_this<tri_mesh>
{
public:
    class instance
    {
    public:
        explicit instance(const std::shared_ptr<const tri_mesh>& geo);

        void render(const camera& camera) const;

        std::shared_ptr<const tri_mesh> mesh;
        gfx::transform                  transform;

        struct
        {
            glm::vec3 color     = glm::vec3(1.f);
            float     roughness = 0.2f;
        } material;

    private:
        struct model_data
        {
            glm::mat4          matrix;
            decltype(material) material;
        };

        int                    _info_index = 0;
        mutable gfx::transform _last_transform;
    };

    explicit tri_mesh(const std::vector<gfx::vertex3d>& vertices,
                      const std::vector<gfx::index32>&  indices = {});

    std::unique_ptr<instance> instantiate() const;

private:
    void render() const;

    std::function<void()> _draw;
    gl::vertex_array      _vertex_array;
    int                   _base_vertex;
    int                   _base_index;
    int                   _draw_count;
    bounds3f              _bounds;
};
}
