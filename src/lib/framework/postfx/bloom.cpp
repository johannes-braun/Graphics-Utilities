#include "bloom.hpp"

namespace gfx::fx
{ 
    bloom::bloom()
    {
        _blur_pipeline.use_stages(screen_vertex_shader(), new gl::shader("postprocess/filter/blur.frag"));
        _flare_pipeline.use_stages(screen_vertex_shader(), new gl::shader("postprocess/filter/flare.frag"));
        _bright_pipeline.use_stages(screen_vertex_shader(), new gl::shader("postprocess/filter/bright_spots.frag"));
        _add_pipeline.use_stages(screen_vertex_shader(), new gl::shader("postprocess/filter/add.frag"));
    }
    void bloom::resize(int x, int y)
    {
        _viewport = { x, y };
        _full_framebuffer = jpu::make_ref<gl::framebuffer>();
        _quarter_framebuffer = jpu::make_ref<gl::framebuffer>();

        _quarter_attachments[0] = jpu::make_ref<gl::texture>(GL_TEXTURE_2D);
        _quarter_attachments[0]->storage_2d(x / 4, y / 4, GL_RGBA16F, 1);
        _quarter_framebuffer->attach(GL_COLOR_ATTACHMENT0, _quarter_attachments[0]);

        _quarter_attachments[1] = jpu::make_ref<gl::texture>(GL_TEXTURE_2D);
        _quarter_attachments[1]->storage_2d(x / 4, y / 4, GL_RGBA16F, 1);
        _quarter_framebuffer->attach(GL_COLOR_ATTACHMENT1, _quarter_attachments[1]);

        _full_attachment = jpu::make_ref<gl::texture>(GL_TEXTURE_2D);
        _full_attachment->storage_2d(x, y, GL_RGBA16F, 1);
        _full_framebuffer->attach(GL_COLOR_ATTACHMENT0, _full_attachment);
    }
    void bloom::run(const std::array<jpu::ref_ptr<gl::texture>, 2>& base_attachments, postprocess_provider & provider, double delta_time)
    {
        provider.begin_draw();
        provider.last_target()->generate_mipmaps();

        glViewportIndexedf(0, 0, 0, _viewport.x / 4, _viewport.y / 4);

        _quarter_framebuffer->bind();
        _quarter_framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT0 });
        _bright_pipeline.bind();
        _bright_pipeline.get_uniform<uint64_t>(gl::shader_type::fragment, "src_textures[0]") = sampler.sample_texture(provider.last_target());
        _bright_pipeline.get_uniform<float>(gl::shader_type::fragment, "threshold_lower") = 1.0f;
        _bright_pipeline.get_uniform<float>(gl::shader_type::fragment, "threshold_higher") = 1.45f;
        _bright_pipeline.draw(gl::primitive::triangles, 3);

        _blur_pipeline.bind();
        _blur_pipeline.get_uniform<uint64_t>(gl::shader_type::fragment, "src_textures[0]") = sampler.sample_texture(_quarter_attachments[0]);
        _blur_pipeline.get_uniform<int>(gl::shader_type::fragment, "axis") = 0;
        _blur_pipeline.get_uniform<int>(gl::shader_type::fragment, "level") = 0;
        _blur_pipeline.draw(gl::primitive::triangles, 3);

        _blur_pipeline.get_uniform<uint64_t>(gl::shader_type::fragment, "src_textures[0]") = sampler.sample_texture(_quarter_attachments[0]);
        _blur_pipeline.get_uniform<int>(gl::shader_type::fragment, "axis") = 1;
        _blur_pipeline.get_uniform<int>(gl::shader_type::fragment, "level") = 0;
        _blur_pipeline.draw(gl::primitive::triangles, 3);
        _quarter_framebuffer->unbind();

        glViewportIndexedf(0, 0, 0, _viewport.x, _viewport.y);

        _full_framebuffer->bind();
        _full_framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT0 });
        _flare_pipeline.bind();
        _flare_pipeline.get_uniform<uint64_t>(gl::shader_type::fragment, "src_textures[0]") = sampler.sample_texture(_quarter_attachments[0]);
        _flare_pipeline.draw(gl::primitive::triangles, 3);
        _full_framebuffer->unbind();

        provider.framebuffer()->bind();
        _add_pipeline.bind();
        _add_pipeline.get_uniform<uint64_t>(gl::shader_type::fragment, "src_textures[0]") = sampler.sample_texture(provider.last_target());
        _add_pipeline.get_uniform<uint64_t>(gl::shader_type::fragment, "src_textures[1]") = sampler.sample_texture(_full_attachment);
        _add_pipeline.get_uniform<float>(gl::shader_type::fragment, "factor_one") = 1.f;
        _add_pipeline.get_uniform<float>(gl::shader_type::fragment, "factor_two") = 0.2f;
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