#include "fxaa.hpp"

namespace gfx::fx
{
    fxaa::fxaa()
    {
        _fxaa_pipeline[GL_VERTEX_SHADER] = screen_vertex_shader();
        _fxaa_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("postfx/filter/fxaa.frag");
    }
    void fxaa::resize(int x, int y)
    {
    }
    void fxaa::run(const std::array<std::shared_ptr<gl::texture>, 2>& base_attachments, postprocess_provider & provider, double delta_time)
    {
        provider.begin_draw();
        _fxaa_pipeline.bind();
        _fxaa_pipeline[GL_FRAGMENT_SHADER]->uniform<uint64_t>("src_textures[0]") = sampler.sample(provider.last_target());
        _fxaa_pipeline.draw(GL_TRIANGLES, 3);
        provider.end_draw();
    }
    void fxaa::reload_pipelines()
    {
        _fxaa_pipeline.reload();
    }
}