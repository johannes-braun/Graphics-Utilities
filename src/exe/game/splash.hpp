#pragma once

#include "ui.hpp"
#include "res/image.hpp"
#include <io/window.hpp>

namespace game
{
    class splash
    {
    public:
        splash(const std::shared_ptr<io::window>& window) : _window(window) {
            const res::image img = res::load_svg_rasterized("../res/ui/logo.svg", 20.f);
            _logo = gl::texture(GL_TEXTURE_2D, img.width, img.height, GL_RGBA8);
            _logo.assign(GL_RGBA, GL_UNSIGNED_BYTE, img.data.get());
            _logo.generate_mipmaps();

            const res::image bg = res::load_image("../res/board.png", res::image_type::u8, res::RGBA);
            _background = gl::texture(GL_TEXTURE_2D, bg.width, bg.height, GL_RGBA8);
            _background.assign(GL_RGBA, GL_UNSIGNED_BYTE, bg.data.get());
            _background.generate_mipmaps();

            update();
            _window->update();
        }

        void set_progress(float f)
        {
            const float step = 0.5f;
            const double sd = _window->get_swap_delay();
            _window->set_max_framerate(60.f);
            update();
            _window->update();
            update();
            while (_progress != f && _window->update())
            {
                update();
                _progress = std::min<float>(_progress + 10.f * step * _window->delta_time(), f);
            }
            _window->update();
            _window->set_swap_delay(sd);
        }

        void update()
        {
            int size = 256 * _progress;
            glm::vec4 start(0, 0, 0, 1);
            glm::vec4 end(0.1f, 0.4f, 0.01f, 1.f);
            glm::vec4 mid = mix(start, end, _progress);
            glClearNamedFramebufferfv(gl_framebuffer_t::zero, GL_COLOR, 0, &mid[0]);

            int w, h; glfwGetFramebufferSize(*_window, &w, &h);
            default_ui().draw_quad({ 0, 0 }, { w,h }, { 0, 0 }, { w / 8, h / 8 }, _background);
            default_ui().draw_quad({ w / 2 - size / 2, h / 2 - size / 2 }, { w / 2 + size / 2, h / 2 + size / 2 }, { 0, 1 }, { 1, 0 }, _logo);

            // progress bar
            default_ui().draw_quad({ 0, 0 }, { w, 24.f }, { 255, 255, 255, 64 });
            default_ui().draw_quad({ 0, 0 }, { _progress * w, 24.f }, { 255, 255, 255, 255 });
            default_ui().draw();
        }

    private:
        std::shared_ptr<io::window> _window;
        float _progress = 0.f;
        gl::texture _logo { GL_TEXTURE_2D };
        gl::texture _background { GL_TEXTURE_2D };
    };
}