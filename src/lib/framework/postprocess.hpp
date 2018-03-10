#pragma once

#include <array>
#include <random>
#include <jpu/memory>
#include <opengl/texture.hpp>
#include <opengl/framebuffer.hpp>
#include <opengl/shader.hpp>
#include <glm/glm.hpp>

namespace gfx
{
    class postprocess_provider
    {
    public:
        postprocess_provider(const glm::ivec2& resolution, int max_mipmaps = 3)
            : _full_resolution(resolution), _max_mipmaps(max_mipmaps)
        {
            resize(resolution, max_mipmaps);
            reset();
        }

        void resize(const glm::ivec2& resolution, int max_mipmaps = 3)
        {
            _full_resolution = resolution;

            _double_buffer[0] = jpu::make_ref<gl::texture>(GL_TEXTURE_2D);
            _double_buffer[0]->storage_2d(_full_resolution.x, _full_resolution.y, GL_RGBA16F, max_mipmaps);
            _double_buffer[1] = jpu::make_ref<gl::texture>(GL_TEXTURE_2D);
            _double_buffer[1]->storage_2d(_full_resolution.x, _full_resolution.y, GL_RGBA16F, max_mipmaps);

            _framebuffer = jpu::make_ref<gl::framebuffer>();
            _framebuffer->attach(GL_COLOR_ATTACHMENT0, _double_buffer[0]);
            _framebuffer->attach(GL_COLOR_ATTACHMENT1, _double_buffer[1]);
        }

        gl::framebuffer* framebuffer() const { return _framebuffer; }
        gl::texture* last_target() const { 
            return _double_buffer[_last_target]; 
        }
        int current_index() const {
            return _target_buffer; 
        }

        void begin_draw()
        {
            _last_target = _target_buffer;
            framebuffer()->draw_to_attachments({ GL_COLOR_ATTACHMENT0 + GLenum(_target_buffer) });
            framebuffer()->read_from_attachment(GL_COLOR_ATTACHMENT0 + GLenum(_target_buffer));
            framebuffer()->bind();
        }

        void end_draw()
        {
            framebuffer()->unbind();
        }
        
        void swap()
        {
            _last_target = _target_buffer;
            _target_buffer = (_target_buffer + 1) % 2;
            framebuffer()->draw_to_attachments({ GL_COLOR_ATTACHMENT0 + GLenum(_target_buffer) });
            framebuffer()->read_from_attachment(GL_COLOR_ATTACHMENT0 + GLenum(_target_buffer));
        }

        void reset()
        {
            _target_buffer = 0;
            framebuffer()->read_from_attachment(GL_COLOR_ATTACHMENT0 + GLenum(_target_buffer));
            framebuffer()->draw_to_attachments({ GL_COLOR_ATTACHMENT0 + GLenum(_target_buffer) });
        }

    private:
        glm::ivec2 _full_resolution{ 0, 0 };
        int _max_mipmaps = 0;
        int _target_buffer = 0;
        int _last_target = 0;
        jpu::ref_ptr<gl::framebuffer> _framebuffer;
        std::array<jpu::ref_ptr<gl::texture>, 2> _double_buffer;
    };

    class postprocess_pass
    {
        public:
            virtual void run(const std::array<jpu::ref_ptr<gl::texture>, 2>& base_attachments, postprocess_provider& provider, double delta_time) = 0;
            virtual void resize(int x, int y) = 0;
            virtual void reload_pipelines() = 0;
            gl::shader* screen_vertex_shader() const {
                static gl::shader shader("postprocess/screen.vert");
                return &shader;
            }
            gl::texture* random_texture() const {
                static jpu::ref_ptr<gl::texture> tex = []() {
                    auto texture = jpu::make_ref<gl::texture>(GL_TEXTURE_2D);
                    texture->storage_2d(512, 512, GL_RGBA16F, 1);
                    std::vector<float> random_pixels(512 * 512 * 4);
                    std::generate(random_pixels.begin(), random_pixels.end(),
                        [gen = std::mt19937(), dist = std::uniform_real_distribution<float>(0.f, 1.f)]() mutable { return dist(gen); });
                    texture->assign_2d(GL_RGBA, GL_FLOAT, random_pixels.data());
                    texture->generate_mipmaps();
                    return texture;
                }();
                return tex;
            }

    protected:
        gl::sampler sampler;
    };
}