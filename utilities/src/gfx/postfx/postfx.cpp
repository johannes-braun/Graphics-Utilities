#include "../postfx.hpp"

namespace gfx
{
    postfx_provider::postfx_provider(const glm::ivec2& resolution, int max_mipmaps)
        : _full_resolution(resolution), _max_mipmaps(max_mipmaps)
    {
        resize(resolution, max_mipmaps);
        reset();
    }

    void postfx_provider::resize(const glm::ivec2& resolution, int max_mipmaps)
    {
        _full_resolution = resolution;

        _double_buffer[0] = std::make_shared<gl::texture>(GL_TEXTURE_2D, _full_resolution.x, _full_resolution.y, GL_RGBA16F, max_mipmaps);
        _double_buffer[1] = std::make_shared<gl::texture>(GL_TEXTURE_2D, _full_resolution.x, _full_resolution.y, GL_RGBA16F, max_mipmaps);

        _framebuffer[GL_COLOR_ATTACHMENT0] = _double_buffer[0];
        _framebuffer[GL_COLOR_ATTACHMENT1] = _double_buffer[1];
    }

    const gl::framebuffer& postfx_provider::framebuffer() const { return _framebuffer; }
    const gl::texture& postfx_provider::last_target() const {
        return *_double_buffer[_last_target];
    }
    int postfx_provider::current_index() const {
        return _target_buffer;
    }

    void postfx_provider::begin_draw()
    {
        _last_target = _target_buffer;
        _framebuffer.set_readbuffer(GL_COLOR_ATTACHMENT0 + GLenum(_target_buffer));
        _framebuffer.set_drawbuffer(GL_COLOR_ATTACHMENT0 + GLenum(_target_buffer));
        _framebuffer.bind();
    }

    void postfx_provider::end_draw()
    {
        gl::framebuffer::zero().bind();
    }

    void postfx_provider::swap()
    {
        _last_target = _target_buffer;
        _target_buffer = (_target_buffer + 1) % 2;
        _framebuffer.set_readbuffer(GL_COLOR_ATTACHMENT0 + GLenum(_target_buffer));
        _framebuffer.set_drawbuffer(GL_COLOR_ATTACHMENT0 + GLenum(_target_buffer));
    }

    void postfx_provider::reset()
    {
        _target_buffer = 0;
        _framebuffer.set_readbuffer(GL_COLOR_ATTACHMENT0 + GLenum(_target_buffer));
        _framebuffer.set_drawbuffer(GL_COLOR_ATTACHMENT0 + GLenum(_target_buffer));
    }

    std::shared_ptr<gl::shader> postfx_pass::screen_vertex_shader() const {
        static auto shader = std::make_shared<gl::shader>("postfx/screen.vert");
        return shader;
    }

    std::shared_ptr<gl::texture> postfx_pass::random_texture() const {
        static std::shared_ptr<gl::texture> tex = []() {
            auto texture = std::make_shared<gl::texture>(GL_TEXTURE_2D, 512, 512, GL_RGBA16F, 1);
            std::vector<float> random_pixels(512 * 512 * 4);
            std::generate(random_pixels.begin(), random_pixels.end(),
                [gen = std::mt19937(), dist = std::uniform_real_distribution<float>(0.f, 1.f)]() mutable { return dist(gen); });
            texture->assign(GL_RGBA, GL_FLOAT, random_pixels.data());
            texture->generate_mipmaps();
            return texture;
        }();
        return tex;
    }
}