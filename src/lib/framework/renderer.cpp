#include "renderer.hpp"
#include <random>
#include <numeric>
#include <GLFW/glfw3.h>
#include <jpu/log>

namespace gfx
{
    renderer::renderer(const int width, const int height, const uint32_t samples)
    {
        _main_framebuffer = jpu::make_ref<gl::framebuffer>();
        _pp_fullsize_framebuffer = jpu::make_ref<gl::framebuffer>();
        _pp_quartersize_framebuffer = jpu::make_ref<gl::framebuffer>();
        resize(width, height, samples);

        _empty_vao = jpu::make_ref<gl::vertex_array>();
        _sampler = jpu::make_ref<gl::sampler>();
        _sampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        _sampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        _sampler->set(GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
        _sampler->set(GL_TEXTURE_CUBE_MAP_SEAMLESS, 16);
        _sampler->set(GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
        _sampler->set(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        _sampler->set(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        _random_texture = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
        _random_texture->storage_2d(512, 512, GL_RGBA16F, 1);
        std::vector<float> random_pixels(512 * 512 * 4);
        std::generate(random_pixels.begin(), random_pixels.end(),
            [gen = std::mt19937(), dist = std::uniform_real_distribution<float>(0.f, 1.f)]() mutable { return dist(gen); });
        _random_texture->assign_2d(GL_RGBA, GL_FLOAT, random_pixels.data());
        _random_texture->generate_mipmaps();

        _temporal_target = GL_COLOR_ATTACHMENT0;

        _luminance_compute_pipeline = jpu::make_ref<gl::compute_pipeline>(new gl::shader(gl::shader_root / "postprocess/tonemap/sample_luminance.comp"));
        _luminance_sample_buffer = jpu::make_ref<gl::buffer>(std::array<float, 32>{}, gl::buffer_flag_bits::map_dynamic_persistent_read);
        _luminance = 0;

        const auto screen_vertex_shader = jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/screen.vert");
        _tonemap_pipeline = jpu::make_ref<gl::graphics_pipeline>();
        _tonemap_pipeline->use_stages(screen_vertex_shader, jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/filter/const_multiply.frag"));
        _blur_pipeline = jpu::make_ref<gl::graphics_pipeline>();
        _blur_pipeline->use_stages(screen_vertex_shader, jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/filter/blur.frag"));
        _fxaa_pipeline = jpu::make_ref<gl::graphics_pipeline>();
        _fxaa_pipeline->use_stages(screen_vertex_shader, jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/filter/fxaa.frag"));
        _flare_pipeline = jpu::make_ref<gl::graphics_pipeline>();
        _flare_pipeline->use_stages(screen_vertex_shader, jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/filter/flare.frag"));
        _bright_spots_pipeline = jpu::make_ref<gl::graphics_pipeline>();
        _bright_spots_pipeline->use_stages(screen_vertex_shader, jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/filter/bright_spots.frag"));
        _add_pipeline = jpu::make_ref<gl::graphics_pipeline>();
        _add_pipeline->use_stages(screen_vertex_shader, jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/filter/add.frag"));
        _ssao_pipeline = jpu::make_ref<gl::graphics_pipeline>();
        _ssao_pipeline->use_stages(screen_vertex_shader, jpu::make_ref<gl::shader>(gl::shader_root / "postprocess/filter/ssao.frag"));
    }

    renderer::~renderer()
    {}

    void renderer::resize(int width, int height, const uint32_t samples)
    {
        tlog_i("Renderer") << "Resizing to (" << width << " x " << height << " x " << (1 << samples) << ")";
        if (width * height * samples == 0)
            return;

        const bool samples_changed = (_current_samples != samples);
        const bool resolution_changed = _full_resolution != glm::ivec2{ width, height };

        _full_resolution = { width, height };
        _quarter_resolution = _full_resolution / 4;
        _current_samples = samples;

        if (resolution_changed)
        {
            tlog_i("Renderer") << "Creating unsampled framebuffers";
            _quarter_size_attachments[0] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
            _quarter_size_attachments[0]->storage_2d(_quarter_resolution.x, _quarter_resolution.y, GL_R11F_G11F_B10F, 1);
            _pp_quartersize_framebuffer->attach(GL_COLOR_ATTACHMENT0, _quarter_size_attachments[0]);

            _quarter_size_attachments[1] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
            _quarter_size_attachments[1]->storage_2d(_quarter_resolution.x, _quarter_resolution.y, GL_R11F_G11F_B10F, 1);
            _pp_quartersize_framebuffer->attach(GL_COLOR_ATTACHMENT1, _quarter_size_attachments[1]);

            _full_size_attachments[0] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
            _full_size_attachments[0]->storage_2d(_full_resolution.x, _full_resolution.y, GL_R11F_G11F_B10F, 3);
            _pp_fullsize_framebuffer->attach(GL_COLOR_ATTACHMENT0, _full_size_attachments[0]);

            _full_size_attachments[1] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
            _full_size_attachments[1]->storage_2d(_full_resolution.x, _full_resolution.y, GL_R11F_G11F_B10F, 1);
            _pp_fullsize_framebuffer->attach(GL_COLOR_ATTACHMENT1, _full_size_attachments[1]);

            _full_size_attachments[2] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
            _full_size_attachments[2]->storage_2d(_full_resolution.x, _full_resolution.y, GL_R11F_G11F_B10F, 1);
            _pp_fullsize_framebuffer->attach(GL_COLOR_ATTACHMENT2, _full_size_attachments[2]);

            _full_size_attachments[3] = jpu::make_ref<gl::texture>(gl::texture_type::def_2d);
            _full_size_attachments[3]->storage_2d(_full_resolution.x, _full_resolution.y, GL_R11F_G11F_B10F, 1);
            _pp_fullsize_framebuffer->attach(GL_COLOR_ATTACHMENT3, _full_size_attachments[3]);
        }

        if (resolution_changed || samples_changed)
        {
            tlog_i("Renderer") << "Creating sampled framebuffers";
            _msaa_attachments[0] = jpu::make_ref<gl::texture>(gl::texture_type::multisample_2d);
            _msaa_attachments[0]->storage_2d_multisample(_full_resolution.x, _full_resolution.y, samples, GL_RGBA16F);
            _main_framebuffer->attach(GL_COLOR_ATTACHMENT0, _msaa_attachments[0]);

            _msaa_attachments[1] = jpu::make_ref<gl::texture>(gl::texture_type::multisample_2d);
            _msaa_attachments[1]->storage_2d_multisample(_full_resolution.x, _full_resolution.y, samples, GL_RGBA16F);
            _main_framebuffer->attach(GL_COLOR_ATTACHMENT1, _msaa_attachments[1]);

            _main_framebuffer->use_renderbuffer_multisample(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, _full_resolution.x, _full_resolution.y, samples);
        }
    }

    void renderer::reload_pipelines() const
    {
        _luminance_compute_pipeline->reload_stages();
        _tonemap_pipeline->reload_stages();
        _blur_pipeline->reload_stages();
        _fxaa_pipeline->reload_stages();
        _flare_pipeline->reload_stages();
        _bright_spots_pipeline->reload_stages();
        _add_pipeline->reload_stages();
        _ssao_pipeline->reload_stages();
    }

    void renderer::bind() const
    {
        _main_framebuffer->bind();
        _main_framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void renderer::draw(const double delta_time)
    {
        _main_framebuffer->unbind();

        _main_framebuffer->read_from_attachment(GL_COLOR_ATTACHMENT0);
        _pp_fullsize_framebuffer->draw_to_attachments({ _temporal_target });
        _main_framebuffer->blit(_pp_fullsize_framebuffer,
            gl::framebuffer::blit_rect{ 0, 0, _full_resolution.x, _full_resolution.y },
            gl::framebuffer::blit_rect{ 0, 0, _full_resolution.x, _full_resolution.y }, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        constexpr auto adaption_speed = 0.4f;
        constexpr auto brightness = 5.0f;

        const auto new_luma = std::accumulate(_luminance_sample_buffer->data_as<float>(), _luminance_sample_buffer->data_as<float>() + 32, 0.f) * (1 / 32.f);
        _luminance = glm::mix(glm::mix(_luminance, new_luma, adaption_speed * delta_time), new_luma, adaption_speed * delta_time);
        const auto half_inv_luma = 0.5f / _luminance;
        const auto luminance = half_inv_luma * (half_inv_luma * brightness + 1.f) / (half_inv_luma + 1.f);
        _luminance_sample_buffer->bind(1, GL_SHADER_STORAGE_BUFFER);
        _luminance_compute_pipeline->bind();
        _luminance_compute_pipeline->stage(gl::shader_type::compute)->get_uniform<gl::sampler2D>("src_texture") = _sampler->sample_texture(_full_size_attachments[0]);
        _luminance_compute_pipeline->dispatch(1);

        _empty_vao->bind();
        _pp_fullsize_framebuffer->bind();
        _pp_fullsize_framebuffer->draw_to_attachments({ _temporal_target });
        _tonemap_pipeline->bind();
        _tonemap_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[0]") = _sampler->sample_texture(_full_size_attachments[0]);
        _tonemap_pipeline->stage(gl::shader_type::fragment)->get_uniform<float>("factor") = luminance;
        glDrawArrays(GL_TRIANGLES, 0, 3);
        _pp_fullsize_framebuffer->unbind();
        _full_size_attachments[0]->generate_mipmaps();

        _pp_quartersize_framebuffer->bind();
        _pp_quartersize_framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT0 });
        glViewport(0, 0, _quarter_resolution.x, _quarter_resolution.y);
        _bright_spots_pipeline->bind();
        _bright_spots_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[0]") = _sampler->sample_texture(_full_size_attachments[0]);
        _bright_spots_pipeline->stage(gl::shader_type::fragment)->get_uniform<float>("threshold_lower") = 1.0f;
        _bright_spots_pipeline->stage(gl::shader_type::fragment)->get_uniform<float>("threshold_higher") = 1.75f;
        glDrawArrays(GL_TRIANGLES, 0, 3);
        _pp_quartersize_framebuffer->unbind();

        _pp_quartersize_framebuffer->bind();
        _pp_quartersize_framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT1 });
        _blur_pipeline->bind();
        _blur_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[0]") = _sampler->sample_texture(_quarter_size_attachments[0]);
        _blur_pipeline->stage(gl::shader_type::fragment)->get_uniform<int>("axis") = 0;
        _blur_pipeline->stage(gl::shader_type::fragment)->get_uniform<int>("level") = 0;
        glDrawArrays(GL_TRIANGLES, 0, 3);
        _pp_quartersize_framebuffer->unbind();

        _pp_quartersize_framebuffer->bind();
        _pp_quartersize_framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT0 });
        _blur_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[0]") = _sampler->sample_texture(_quarter_size_attachments[1]);
        _blur_pipeline->stage(gl::shader_type::fragment)->get_uniform<int>("axis") = 1;
        _blur_pipeline->stage(gl::shader_type::fragment)->get_uniform<int>("level") = 0;
        glDrawArrays(GL_TRIANGLES, 0, 3);
        _pp_quartersize_framebuffer->unbind();

        _pp_fullsize_framebuffer->bind();
        _pp_fullsize_framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT1 });
        glViewport(0, 0, _full_resolution.x, _full_resolution.y);
        _flare_pipeline->bind();
        _flare_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[0]") = _sampler->sample_texture(_quarter_size_attachments[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        _pp_fullsize_framebuffer->unbind();

        _pp_fullsize_framebuffer->bind();
        _pp_fullsize_framebuffer->draw_to_attachments({ _temporal_target });
        _add_pipeline->bind();
        _add_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[0]") = _sampler->sample_texture(_full_size_attachments[0]);
        _add_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[1]") = _sampler->sample_texture(_full_size_attachments[1]);
        _add_pipeline->stage(gl::shader_type::fragment)->get_uniform<float>("factor_one") = 1.f;
        _add_pipeline->stage(gl::shader_type::fragment)->get_uniform<float>("factor_two") = 0.3f;
        glDrawArrays(GL_TRIANGLES, 0, 3);
        _pp_fullsize_framebuffer->unbind();

        _pp_fullsize_framebuffer->bind();
        _pp_fullsize_framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT1 });
        _ssao_pipeline->bind();
        _ssao_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("albedo_texture") = _sampler->sample_texture(_full_size_attachments[0]);
        _ssao_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("random_texture") = _sampler->sample_texture(_random_texture);
        _ssao_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("normal_depth_texture") = _msaa_attachments[0]->address();
        _ssao_pipeline->stage(gl::shader_type::fragment)->get_uniform<float>("time") = glfwGetTime();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        _pp_fullsize_framebuffer->unbind();

        _pp_fullsize_framebuffer->bind();
        _pp_fullsize_framebuffer->draw_to_attachments({ _temporal_target });
        _fxaa_pipeline->bind();
        _fxaa_pipeline->stage(gl::shader_type::fragment)->get_uniform<gl::sampler2D>("src_textures[0]") = _sampler->sample_texture(_full_size_attachments[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        _pp_fullsize_framebuffer->unbind();

        _pp_fullsize_framebuffer->read_from_attachment(_temporal_target);
        _pp_fullsize_framebuffer->blit(nullptr,
            gl::framebuffer::blit_rect{ 0, 0, _full_resolution.x, _full_resolution.y },
            gl::framebuffer::blit_rect{ 0, 0, _full_resolution.x, _full_resolution.y },
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    gl::texture* renderer::random_texture() const
    {
        return _random_texture;
    }
}
