#pragma once
#include <gfx/geometry.hpp>
#include <memory>
#include <opengl/opengl.hpp>
#include <optional>

namespace gfx
{
struct camera;

struct instance_info
{
    alignas(16) union
    {
        struct
        {
            uint32_t count = 0;
            uint32_t instance_count = 0;
            uint32_t base_index = 0;
            uint32_t base_vertex = 0;
            uint32_t base_instance = 0;
        } elements;

        struct
        {
            uint32_t count;
            uint32_t instance_count;
            uint32_t base_vertex;
            uint32_t base_instance;
        } arrays;
    } indirect {};

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
    gl::vertex_array      _vertex_array;
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

        int _info_index = 0;
        mutable gfx::transform         _last_transform;
        //mutable gl::buffer<model_data> _model_buffer;
    };

    explicit tri_mesh(const std::vector<gfx::vertex3d>& vertices, const std::vector<gfx::index32>& indices = {});

    std::unique_ptr<instance> instantiate() const;

private:
    void render() const;

    std::function<void()> _draw;
    gl::vertex_array      _vertex_array;
    int                   _base_vertex;
    int                   _base_index;
    int _draw_count;
    bounds3f              _bounds;
};
}
