#pragma once

#include <array>
#include <random>
#include <opengl/texture.hpp>
#include <opengl/framebuffer.hpp>
#include <opengl/shader.hpp>
#include <glm/glm.hpp>

namespace gfx
{
    class postfx_provider
    {
    public:
        postfx_provider(const glm::ivec2& resolution, int max_mipmaps = 3);

        void begin_draw();
        void end_draw();
        void swap();
        void reset();

        void resize(const glm::ivec2& resolution, int max_mipmaps = 3);

        const gl::framebuffer& framebuffer() const;
        const gl::texture& last_target() const;
        int current_index() const;

    private:
        glm::ivec2 _full_resolution{ 0, 0 };
        int _max_mipmaps = 0;
        int _target_buffer = 0;
        int _last_target = 0;
        gl::framebuffer _framebuffer;
        std::array<std::shared_ptr<gl::texture>, 2> _double_buffer;
    };

    class postfx_pass
    {
    public:
        virtual ~postfx_pass() = default;
        virtual void run(const std::array<std::shared_ptr<gl::texture>, 2>& base_attachments, postfx_provider& provider, double delta_time) = 0;
        virtual void resize(int x, int y) = 0;
        virtual void reload_pipelines() = 0;
        std::shared_ptr<gl::shader> screen_vertex_shader() const;
        std::shared_ptr<gl::texture> random_texture() const;

    protected:
        gl::sampler sampler;
    };
}