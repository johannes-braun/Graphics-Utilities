#include "ssao.hpp"
#include <random>
#include <GLFW/glfw3.h>

namespace gfx::fx
{
    ssao::ssao()
    {
        _ssao_pipeline.use_stages(screen_vertex_shader(), new gl::shader("postprocess/filter/ssao.frag"));
    }
    void ssao::resize(int x, int y)
    {
    }
    void ssao::run(const std::array<jpu::ref_ptr<gl::texture>, 2>& base_attachments, postprocess_provider & provider, double delta_time)
    {
        provider.begin_draw();
        _ssao_pipeline.bind();
        _ssao_pipeline.get_uniform<uint64_t>(gl::shader_type::fragment, "albedo_texture") = sampler.sample_texture(provider.last_target());
        _ssao_pipeline.get_uniform<uint64_t>(gl::shader_type::fragment, "random_texture") = sampler.sample_texture(random_texture());
        _ssao_pipeline.get_uniform<uint64_t>(gl::shader_type::fragment, "normal_depth_texture") = base_attachments[1]->address();
        _ssao_pipeline.get_uniform<float>(gl::shader_type::fragment, "time") = static_cast<float>(glfwGetTime());
        _ssao_pipeline.draw(gl::primitive::triangles, 3);
        provider.end_draw();
    }
    void ssao::reload_pipelines()
    {
        _ssao_pipeline.reload_stages();
    }
}