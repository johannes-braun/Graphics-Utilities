#pragma once

#include "../postprocess.hpp"
#include <opengl/pipeline.hpp>

namespace gfx::fx
{
    class fxaa : public postprocess_pass
    {
    public:
        fxaa();
        virtual void resize(int x, int y) override;
        virtual void run(const std::array<std::shared_ptr<gl::v2::texture>, 2>& base_attachments, postprocess_provider & provider, double delta_time) override;
        virtual void reload_pipelines() override;

    private:
        gl::graphics_pipeline _fxaa_pipeline;
    };
}