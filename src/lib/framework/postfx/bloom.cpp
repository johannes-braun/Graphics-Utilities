#include "bloom.hpp"

namespace gfx::fx
{ 
    bloom::bloom()
    {
        _blur_pipeline.use_stages(screen_vertex_shader(), std::make_shared<gl::shader>("postprocess/filter/blur.frag"));
        _flare_pipeline.use_stages(screen_vertex_shader(), std::make_shared<gl::shader>("postprocess/filter/flare.frag"));
        _bright_pipeline.use_stages(screen_vertex_shader(), std::make_shared<gl::shader>("postprocess/filter/bright_spots.frag"));
        _add_pipeline.use_stages(screen_vertex_shader(), std::make_shared<gl::shader>("postprocess/filter/add.frag"));
    }
    void bloom::resize(int x, int y)
    {
        _viewport = { x, y };

        _quarter_attachments[0] = std::make_shared<gl::texture>(GL_TEXTURE_2D, x / 4, y / 4, GL_RGBA16F, 1);
        _quarter_attachments[1] = std::make_shared<gl::texture>(GL_TEXTURE_2D, x / 4, y / 4, GL_RGBA16F, 1);
        _full_attachment = std::make_shared<gl::texture>(GL_TEXTURE_2D, x, y, GL_RGBA16F, 1);

        _quarter_framebuffer[GL_COLOR_ATTACHMENT0] = _quarter_attachments[0];
        _quarter_framebuffer[GL_COLOR_ATTACHMENT1] = _quarter_attachments[1];
        _full_framebuffer[GL_COLOR_ATTACHMENT0] = _full_attachment;
    }
    void bloom::run(const std::array<std::shared_ptr<gl::texture>, 2>& base_attachments, postprocess_provider & provider, double delta_time)
    {
        provider.begin_draw();
        provider.last_target().generate_mipmaps();

        glViewportIndexedf(0, 0, 0, _viewport.x / 4, _viewport.y / 4);

        _quarter_framebuffer.bind();
        _quarter_framebuffer.set_drawbuffer(GL_COLOR_ATTACHMENT0);
        _bright_pipeline.bind();
        _bright_pipeline.get_uniform<uint64_t>(GL_FRAGMENT_SHADER, "src_textures[0]") = sampler.sample(provider.last_target());
        _bright_pipeline.get_uniform<float>(GL_FRAGMENT_SHADER, "threshold_lower") = 1.0f;
        _bright_pipeline.get_uniform<float>(GL_FRAGMENT_SHADER, "threshold_higher") = 1.45f;
        _bright_pipeline.draw(gl::primitive::triangles, 3);

        _quarter_framebuffer.set_drawbuffer(GL_COLOR_ATTACHMENT1);
        _blur_pipeline.bind();
        _blur_pipeline.get_uniform<uint64_t>(GL_FRAGMENT_SHADER, "src_textures[0]") = sampler.sample(*_quarter_attachments[0]);
        _blur_pipeline.get_uniform<int>(GL_FRAGMENT_SHADER, "axis") = 0;
        _blur_pipeline.get_uniform<int>(GL_FRAGMENT_SHADER, "level") = 0;
        _blur_pipeline.draw(gl::primitive::triangles, 3);

        _quarter_framebuffer.set_drawbuffer(GL_COLOR_ATTACHMENT0);
        _blur_pipeline.get_uniform<uint64_t>(GL_FRAGMENT_SHADER, "src_textures[0]") = sampler.sample(*_quarter_attachments[1]);
        _blur_pipeline.get_uniform<int>(GL_FRAGMENT_SHADER, "axis") = 1;
        _blur_pipeline.get_uniform<int>(GL_FRAGMENT_SHADER, "level") = 0;
        _blur_pipeline.draw(gl::primitive::triangles, 3);

        glViewportIndexedf(0, 0, 0, _viewport.x, _viewport.y);

        _full_framebuffer.bind();
        _full_framebuffer.set_drawbuffer(GL_COLOR_ATTACHMENT0);
        _flare_pipeline.bind();
        _flare_pipeline.get_uniform<uint64_t>(GL_FRAGMENT_SHADER, "src_textures[0]") = sampler.sample(*_quarter_attachments[0]);
        _flare_pipeline.draw(gl::primitive::triangles, 3);

        provider.framebuffer().bind();
        _add_pipeline.bind();
        _add_pipeline.get_uniform<uint64_t>(GL_FRAGMENT_SHADER, "src_textures[0]") = sampler.sample(provider.last_target());
        _add_pipeline.get_uniform<uint64_t>(GL_FRAGMENT_SHADER, "src_textures[1]") = sampler.sample(*_full_attachment);
        _add_pipeline.get_uniform<float>(GL_FRAGMENT_SHADER, "factor_one") = 1.f;
        _add_pipeline.get_uniform<float>(GL_FRAGMENT_SHADER, "factor_two") = 0.2f;
        _add_pipeline.draw(gl::primitive::triangles, 3);
        provider.end_draw();
    }
    void bloom::reload_pipelines()
    {
        _bright_pipeline.reload_stages();
        _blur_pipeline.reload_stages();
        _flare_pipeline.reload_stages();
        _add_pipeline.reload_stages();
    }
}