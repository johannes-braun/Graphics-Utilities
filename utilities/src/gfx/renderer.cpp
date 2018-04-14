#include "renderer.hpp"
#include <random>
#include <numeric>
#include <GLFW/glfw3.h>
#include "log.hpp"
#include "postfx/bloom.hpp"
#include "postfx/ssao.hpp"
#include "postfx/fxaa.hpp"
#include "postfx/auto_exposure.hpp"

namespace gfx
{
    renderer::renderer(const int width, const int height, const uint32_t samples)
    {
        _passes.push_back({ "Auto Exposure", std::make_shared<gfx::fx::auto_exposure>(), true });
        _passes.push_back({ "SSAO", std::make_shared<gfx::fx::ssao>(), false });
        _passes.push_back({ "Bloom", std::make_shared<gfx::fx::bloom>(), true });
        _passes.push_back({ "FXAA", std::make_shared<gfx::fx::fxaa>(), true });

        _pp_provider = std::make_shared<postprocess_provider>(glm::ivec2{width, height}, 3);
        resize(width, height, samples);
    }

    renderer::~renderer()
    {}

    void renderer::resize(int width, int height, const uint32_t samples)
    {
        if (width * height * samples == 0)
            return;

        const bool samples_changed = (_current_samples != samples);
        const bool resolution_changed = _full_resolution != glm::ivec2{ width, height };

        _full_resolution = { width, height };
        _current_samples = samples;

        if (resolution_changed)
        {
            _pp_provider->resize(_full_resolution, 3);
            for (auto&& pass : _passes)
                pass.p->resize(width, height);
        }

        if (resolution_changed || samples_changed)
        {
            tlog_i("Renderer") << "Resizing to (" << width << " x " << height << " x " << samples << ")";
            _msaa_attachments[0] = std::make_shared<gl::texture>(GL_TEXTURE_2D_MULTISAMPLE, _full_resolution.x, _full_resolution.y, gl::samples(uint32_t(std::log2(samples))), GL_RGBA16F);
            _msaa_attachments[1] = std::make_shared<gl::texture>(GL_TEXTURE_2D_MULTISAMPLE, _full_resolution.x, _full_resolution.y, gl::samples(uint32_t(std::log2(samples))), GL_RGBA16F);
            _main_framebuffer[GL_COLOR_ATTACHMENT0] = _msaa_attachments[0];
            _main_framebuffer[GL_COLOR_ATTACHMENT1] = _msaa_attachments[1];
            _main_framebuffer[GL_DEPTH_STENCIL_ATTACHMENT] = std::make_shared<gl::renderbuffer>(GL_DEPTH24_STENCIL8, _full_resolution.x, _full_resolution.y, gl::samples(uint32_t(std::log2(samples))));

        }
        glViewportIndexedf(0, 0, 0, float(width), float(height));
    }

    void renderer::reload_pipelines()
    {
        for (auto&& pass : _passes)
            pass.p->reload_pipelines();
    }

    void renderer::set_clear_color(const glm::vec4 color)
    {
        _clear_color = color;
    }

    void renderer::set_clear_depth(const float depth)
    {
        _clear_depth = depth;
    }

    void renderer::bind()
    {
        _main_framebuffer.bind();
        _main_framebuffer.set_drawbuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });
        _main_framebuffer.clear(0, _clear_color);
        _main_framebuffer.clear(1, { 1, 0, 0, 1 });
        _main_framebuffer.clear(_clear_depth, 0);
    }

    void renderer::draw(const double delta_time, const gl::framebuffer& target_framebuffer)
    {
        _pp_provider->reset();
        _main_framebuffer.blit(_pp_provider->framebuffer(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

        if (enabled)
            for (auto&& pass : _passes)
                if(pass.enabled) pass.p->run(_msaa_attachments, *_pp_provider, delta_time);
        
        _pp_provider->framebuffer().blit(target_framebuffer, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        target_framebuffer.bind();
    }
}
