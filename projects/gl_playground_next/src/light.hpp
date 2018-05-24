#pragma once
#include <gfx/camera.hpp>
#include <gfx/geometry.hpp>
#include <opengl/opengl.hpp>

namespace gfx
{
struct light
{
    gl::sampler                  sampler;
    camera                       map_camera;
    gl::framebuffer              framebuffer;
    std::shared_ptr<gl::texture> depth_attachment;
    int                          map_resolution = 1024;
    glm::vec4                    color          = glm::vec4(1, 1, 1, 15.f);
    bool                         use_shadowmap;

    struct info
    {
        alignas(16) glm::mat4 shadow_matrix = glm::mat4(1.f);
        alignas(16) glm::vec4 position      = glm::vec4(0, 0, 0, 1);
        alignas(16) glm::vec4 direction     = glm::vec4(0);
        alignas(16) glm::vec4 color         = glm::vec4(1, 1, 1, 15.f);
        alignas(16) uint64_t shadow_map     = 0ull;
    };

    light(bool use_shadowmap = true);
    bool begin_shadowmap();
    info make_info() const;

private:
    gl::buffer<camera::data> _matrix_buffer;
};
}
