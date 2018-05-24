#pragma once
#include <gfx/geometry.hpp>
#include <memory>
#include <opengl/opengl.hpp>
#include <optional>

namespace gfx
{
class tri_mesh : public std::enable_shared_from_this<tri_mesh>
{
public:
    class instance
    {
    public:
        explicit instance(const std::shared_ptr<const tri_mesh>& geo);

        void render() const;

        std::shared_ptr<const tri_mesh> mesh;
        gfx::transform                  transform;

    private:
        mutable gfx::transform        _last_transform;
        mutable gl::buffer<glm::mat4> _model_buffer;
    };

    explicit tri_mesh(const std::vector<gfx::vertex3d>& vertices, const std::vector<gfx::index32>& indices = {});

    std::unique_ptr<instance> instantiate() const;

private:
    void                                    render() const;
    std::function<void()>                   _draw;
    gl::buffer<gfx::vertex3d>               _vertex_buffer;
    std::optional<gl::buffer<gfx::index32>> _index_buffer;
    gl::vertex_array                        _vertex_array;
};
}
