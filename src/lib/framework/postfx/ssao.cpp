#include "ssao.hpp"
#include <random>
#include <GLFW/glfw3.h>

namespace gfx::fx
{
    ssao::ssao()
    {
        _ssao_pipeline.use_stages(screen_vertex_shader(), std::make_shared<gl::shader>("postprocess/filter/ssao.frag"));
    }
    void ssao::resize(int x, int y)
    {
    }
    void ssao::run(const std::array<std::shared_ptr<gl::v2::texture>, 2>& base_attachments, postprocess_provider & provider, double delta_time)
    {
        provider.begin_draw();
        _ssao_pipeline.bind();
        _ssao_pipeline.get_uniform<uint64_t>(GL_FRAGMENT_SHADER, "albedo_texture") = sampler.sample(provider.last_target());
        _ssao_pipeline.get_uniform<uint64_t>(GL_FRAGMENT_SHADER, "random_texture") = sampler.sample(*random_texture());
        _ssao_pipeline.get_uniform<uint64_t>(GL_FRAGMENT_SHADER, "normal_depth_texture") = base_attachments[1]->handle();
        _ssao_pipeline.get_uniform<float>(GL_FRAGMENT_SHADER, "time") = static_cast<float>(glfwGetTime());
        _ssao_pipeline.draw(gl::primitive::triangles, 3);
        provider.end_draw();
    }
    void ssao::reload_pipelines()
    {
        _ssao_pipeline.reload_stages();
    }
}