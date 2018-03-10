#include "fxaa.hpp"

namespace gfx::fx
{
    fxaa::fxaa()
    {
        _fxaa_pipeline.use_stages(screen_vertex_shader(), new gl::shader("postprocess/filter/fxaa.frag"));
    }
    void fxaa::resize(int x, int y)
    {
    }
    void fxaa::run(const std::array<jpu::ref_ptr<gl::texture>, 2>& base_attachments, postprocess_provider & provider, double delta_time)
    {
        provider.begin_draw();
        _fxaa_pipeline.bind();
        _fxaa_pipeline.get_uniform<uint64_t>(gl::shader_type::fragment, "src_textures[0]") = sampler.sample_texture(provider.last_target());
        _fxaa_pipeline.draw(gl::primitive::triangles, 3);
        provider.end_draw();
    }
    void fxaa::reload_pipelines()
    {
        _fxaa_pipeline.reload_stages();
    }
}