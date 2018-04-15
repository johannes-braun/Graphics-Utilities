#pragma once

#include <array>
#include <opengl/texture.hpp>
#include <opengl/framebuffer.hpp>
#include <opengl/pipeline.hpp>
#include <opengl/buffer.hpp>
#include <glm/glm.hpp>
#include "postfx.hpp"

namespace gfx
{
    class renderer
    {
    public:
        renderer(int width, int height, uint32_t samples);
        ~renderer();

        void bind();
        void draw(double delta_time, const gl::framebuffer& target_framebuffer = gl::framebuffer::zero());
        void reload_pipelines();

        void resize(int width, int height, uint32_t samples);
        void set_clear_color(glm::vec4 color);
        void set_clear_depth(float depth);

        const gl::framebuffer& main_framebuffer() const;

        bool enabled = true;
        bool enable_tonemap = true;
        bool enable_bloom = true;
        bool enable_ssao = true;
        bool enable_fxaa = true;

    private:

        int _current_samples{ 0 };
        glm::ivec2 _full_resolution{ 0, 0 };

        std::shared_ptr<postfx_provider> _pp_provider;

        gl::framebuffer _main_framebuffer;
        std::array<std::shared_ptr<gl::texture>, 2> _msaa_attachments;

        glm::vec4 _clear_color{0, 0, 0, 1};
        float _clear_depth = 0.f;

        struct pass
        {
            std::string name;
            std::shared_ptr<postfx_pass> p;
            bool enabled = true;
        };
        std::vector<pass> _passes;
    };
}
