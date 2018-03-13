#pragma once

#include <array>
#include <jpu/memory.hpp>
#include <glm/vec2.hpp>
#include "../postprocess.hpp"
#include <opengl/pipeline.hpp>

namespace gfx::fx
{
    class bloom : public postprocess_pass
    {
    public:
        bloom();
        virtual void resize(int x, int y) override;
        virtual void run(const std::array<std::shared_ptr<gl::v2::texture>, 2>& base_attachments, postprocess_provider & provider, double delta_time) override;
        virtual void reload_pipelines() override;

    private:
        jpu::ref_ptr<gl::framebuffer> _quarter_framebuffer;
        jpu::ref_ptr<gl::framebuffer> _full_framebuffer;
        std::array<std::shared_ptr<gl::v2::texture>, 2> _quarter_attachments;
        std::shared_ptr<gl::v2::texture> _full_attachment;
        glm::ivec2 _viewport;
        gl::graphics_pipeline _bright_pipeline;
        gl::graphics_pipeline _blur_pipeline;
        gl::graphics_pipeline _flare_pipeline;
        gl::graphics_pipeline _add_pipeline;
    };
}