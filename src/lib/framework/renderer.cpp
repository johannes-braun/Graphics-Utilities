#include "renderer.hpp"
#include <random>
#include <numeric>
#include <GLFW/glfw3.h>
#include <jpu/log.hpp>
#include "postfx/bloom.hpp"
#include "postfx/ssao.hpp"
#include "postfx/fxaa.hpp"
#include "postfx/auto_exposure.hpp"

namespace gfx
{
    renderer::renderer(const int width, const int height, const uint32_t samples)
    {
        _passes.emplace("Auto Exposure", std::make_pair(std::make_shared<gfx::fx::auto_exposure>(), true));
        _passes.emplace("SSAO", std::make_pair(std::make_shared<gfx::fx::ssao>(), false));
        _passes.emplace("Bloom", std::make_pair(std::make_shared<gfx::fx::bloom>(), true));
        _passes.emplace("FXAA", std::make_pair(std::make_shared<gfx::fx::fxaa>(), true));

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
                pass.first->resize(width, height);
        }

        if (resolution_changed || samples_changed)
        {
            tlog_i("Renderer") << "Resizing to (" << width << " x " << height << " x " << samples << ")";
            _main_framebuffer = jpu::make_ref<gl::framebuffer>();

            _msaa_attachments[0] = jpu::make_ref<gl::texture>(GL_TEXTURE_2D_MULTISAMPLE);
            _msaa_attachments[0]->storage_2d_multisample(_full_resolution.x, _full_resolution.y, samples, GL_RGBA16F);
            _main_framebuffer->attach(GL_COLOR_ATTACHMENT0, _msaa_attachments[0]);

            _msaa_attachments[1] = jpu::make_ref<gl::texture>(GL_TEXTURE_2D_MULTISAMPLE);
            _msaa_attachments[1]->storage_2d_multisample(_full_resolution.x, _full_resolution.y, samples, GL_RGBA16F);
            _main_framebuffer->attach(GL_COLOR_ATTACHMENT1, _msaa_attachments[1]);

            _main_framebuffer->use_renderbuffer_multisample(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, _full_resolution.x, _full_resolution.y, samples);

        }
        glViewportIndexedf(0, 0, 0, width, height);
    }

    void renderer::reload_pipelines()
    {
        for (auto&& pass : _passes)
            pass.first->reload_pipelines();
    }

    void renderer::set_clear_color(const glm::vec4 color)
    {
        _clear_color = color;
    }

    void renderer::set_clear_depth(const float depth)
    {
        _clear_depth = depth;
    }

    void renderer::bind() const
    {
        _main_framebuffer->bind();
        _main_framebuffer->draw_to_attachments({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });
        _main_framebuffer->clear_color(0, _clear_color);
        _main_framebuffer->clear_color(1, { 1, 0, 0, 1 });
        _main_framebuffer->clear_depth(_clear_depth);
    }

    void renderer::draw(const double delta_time, const gl::framebuffer& target_framebuffer)
    {
        _main_framebuffer->unbind();
        _main_framebuffer->read_from_attachment(GL_COLOR_ATTACHMENT0);
        _pp_provider->reset();
        _main_framebuffer->blit(*_pp_provider->framebuffer(),
            gl::framebuffer::blit_rect{ 0, 0, _full_resolution.x, _full_resolution.y }, 
            GL_COLOR_BUFFER_BIT, GL_LINEAR);

        if (enabled)
            for (auto&& pass : _passes)
                if(pass.second) pass.first->run(_msaa_attachments, *_pp_provider, delta_time);
        
        _pp_provider->framebuffer()->blit(target_framebuffer,
            gl::framebuffer::blit_rect{ 0, 0, _full_resolution.x, _full_resolution.y },
            GL_COLOR_BUFFER_BIT, GL_LINEAR);

        target_framebuffer.bind();
    }
}
