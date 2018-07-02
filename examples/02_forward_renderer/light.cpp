#include "light.hpp"

namespace gfx
{
light::light(bool use_shadowmap)
        : use_shadowmap(use_shadowmap)
        , _matrix_buffer(map_camera.info(), GL_DYNAMIC_STORAGE_BIT)
{
    depth_attachment = std::make_shared<gl::texture>(GL_TEXTURE_2D, map_resolution, map_resolution, GL_DEPTH_COMPONENT16, 1);
    framebuffer[GL_DEPTH_ATTACHMENT] = depth_attachment;
    sampler.set(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    sampler.set(GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    sampler.set(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    sampler.set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    sampler.set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

bool light::begin_shadowmap()
{
    if(!use_shadowmap || !enabled)
        return false;
    glViewport(0, 0, map_resolution, map_resolution);
    _matrix_buffer[0] = map_camera.info();
    _matrix_buffer.synchronize();
    framebuffer.bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    _matrix_buffer.bind(GL_UNIFORM_BUFFER, 0);
    return true;
}

light::info light::make_info() const
{
    info i;
    i.shadow_map    = use_shadowmap ? sampler.sample(*depth_attachment) : 0ull;
    i.position      = glm::vec4(map_camera.transform_mode.position, 1);
    i.direction     = glm::mat4(map_camera.transform_mode) * glm::vec4(0, 0, -1, 0);
    i.color         = color;
    i.shadow_matrix = use_shadowmap ? glm::mat4(map_camera.projection_mode) * inverse(map_camera.transform_mode.matrix()) : glm::mat4(1.f);
    return i;
}
}
