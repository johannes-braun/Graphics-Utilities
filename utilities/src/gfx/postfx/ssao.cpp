#include "ssao.hpp"
#include <random>
#include <GLFW/glfw3.h>

namespace gfx::fx
{
    ssao::ssao()
    {
        _ssao_pipeline[GL_VERTEX_SHADER] = screen_vertex_shader();
        _ssao_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("postfx/filter/ssao.frag");
    }
    void ssao::resize(int x, int y)
    {
    }
    void ssao::run(const std::array<std::shared_ptr<gl::texture>, 2>& base_attachments, postprocess_provider & provider, double delta_time)
    {
        provider.begin_draw();
        _ssao_pipeline.bind();
        _ssao_pipeline[GL_FRAGMENT_SHADER]->uniform<uint64_t>("albedo_texture") = sampler.sample(provider.last_target());
        _ssao_pipeline[GL_FRAGMENT_SHADER]->uniform<uint64_t>("random_texture") = sampler.sample(*random_texture());
        _ssao_pipeline[GL_FRAGMENT_SHADER]->uniform<uint64_t>("normal_depth_texture") = base_attachments[1]->handle();
        _ssao_pipeline[GL_FRAGMENT_SHADER]->uniform<float>("time") = static_cast<float>(glfwGetTime());
        _ssao_pipeline.draw(GL_TRIANGLES, 3);
        provider.end_draw();
    }
    void ssao::reload_pipelines()
    {
        _ssao_pipeline.reload();
    }
}