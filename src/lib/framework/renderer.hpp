#pragma once

#include <array>
#include <opengl/texture.hpp>
#include <opengl/framebuffer.hpp>
#include <opengl/pipeline.hpp>
#include <opengl/vertex_array.hpp>
#include <opengl/buffer.hpp>
#include <glm/glm.hpp>

namespace gfx
{
    class renderer
    {
    public:
        renderer(int width, int height, uint32_t samples);
        ~renderer();

        void resize(int width, int height, uint32_t samples);
        void reload_pipelines() const;
        void set_clear_color(glm::vec4 color);
        void set_clear_depth(float depth);

        void bind() const;
        void draw(double delta_time, const gl::framebuffer& target_framebuffer = gl::framebuffer::default_fbo());
        gl::texture* random_texture() const;

        bool enabled = true;
        gl::framebuffer* main_framebuffer() const
        {
            return _main_framebuffer;
        }

    private:

        int _current_samples{ 0 };
        glm::ivec2 _full_resolution{ 0, 0 };
        glm::ivec2 _quarter_resolution{ 0, 0 };

        jpu::ref_ptr<gl::framebuffer> _main_framebuffer;
        jpu::ref_ptr<gl::framebuffer> _pp_fullsize_framebuffer;
        jpu::ref_ptr<gl::framebuffer> _pp_quartersize_framebuffer;
        std::array<jpu::ref_ptr<gl::texture>, 2> _quarter_size_attachments;
        std::array<jpu::ref_ptr<gl::texture>, 4> _full_size_attachments;
        std::array<jpu::ref_ptr<gl::texture>, 2> _msaa_attachments;

        jpu::ref_ptr<gl::sampler> _sampler;
        jpu::ref_ptr<gl::texture> _random_texture;

        glm::vec4 _clear_color{0, 0, 0, 1};
        float _clear_depth = 0.f;
        GLenum _temporal_target;

        jpu::ref_ptr<gl::compute_pipeline> _luminance_compute_pipeline;
        jpu::ref_ptr<gl::buffer> _luminance_sample_buffer;
        float _luminance;
        jpu::ref_ptr<gl::graphics_pipeline> _tonemap_pipeline;
        jpu::ref_ptr<gl::graphics_pipeline> _blur_pipeline;
        jpu::ref_ptr<gl::graphics_pipeline> _fxaa_pipeline;
        jpu::ref_ptr<gl::graphics_pipeline> _flare_pipeline;
        jpu::ref_ptr<gl::graphics_pipeline> _bright_spots_pipeline;
        jpu::ref_ptr<gl::graphics_pipeline> _add_pipeline;
        jpu::ref_ptr<gl::graphics_pipeline> _ssao_pipeline;
    };
}
