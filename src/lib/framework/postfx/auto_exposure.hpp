#pragma once

#include "../postprocess.hpp"
#include <opengl/buffer.hpp>
#include <opengl/pipeline.hpp>

namespace gfx::fx
{
    class auto_exposure : public postprocess_pass
    {
    public:
        auto_exposure();
        virtual void run(const std::array<jpu::ref_ptr<gl::texture>, 2>& base_attachments, postprocess_provider& provider, double delta_time) override;
        virtual void resize(int x, int y) override;
        virtual void reload_pipelines() override;
    private:
        float _luminance;
        gl::compute_pipeline _luminance_compute_pipeline;
        gl::buffer _luminance_sample_buffer;
        gl::graphics_pipeline _tonemap_pipeline;
    };
}