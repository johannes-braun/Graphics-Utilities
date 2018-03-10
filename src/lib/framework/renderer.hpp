#pragma once

#include <array>
#include <opengl/texture.hpp>
#include <opengl/framebuffer.hpp>
#include <opengl/pipeline.hpp>
#include <opengl/vertex_array.hpp>
#include <opengl/buffer.hpp>
#include <jpu/data.hpp>
#include <glm/glm.hpp>
#include "postprocess.hpp"

namespace gfx
{
    class renderer
    {
    public:
        renderer(int width, int height, uint32_t samples);
        ~renderer();

        void resize(int width, int height, uint32_t samples);
        void reload_pipelines();
        void set_clear_color(glm::vec4 color);
        void set_clear_depth(float depth);

        void bind() const;
        void draw(double delta_time, const gl::framebuffer& target_framebuffer = gl::framebuffer::default_fbo());

        gl::framebuffer* main_framebuffer() const
        {
            return _main_framebuffer;
        }

        bool enabled = true;
        bool enable_tonemap = true;
        bool enable_bloom = true;
        bool enable_ssao = true;
        bool enable_fxaa = true;

    private:

        int _current_samples{ 0 };
        glm::ivec2 _full_resolution{ 0, 0 };

        std::shared_ptr<postprocess_provider> _pp_provider;

        jpu::ref_ptr<gl::framebuffer> _main_framebuffer;
        std::array<jpu::ref_ptr<gl::texture>, 2> _msaa_attachments;

        glm::vec4 _clear_color{0, 0, 0, 1};
        float _clear_depth = 0.f;

        jpu::named_vector<std::string, std::pair<std::shared_ptr<postprocess_pass>, bool>> _passes;
    };
}
