#pragma once

#include "../postfx.hpp"
#include <opengl/pipeline.hpp>

namespace gfx::fx
{
    class fxaa : public postfx_pass
    {
    public:
        fxaa();
        virtual void resize(int x, int y) override;
        virtual void run(const std::array<std::shared_ptr<gl::texture>, 2>& base_attachments, postfx_provider & provider, double delta_time) override;
        virtual void reload_pipelines() override;

    private:
        gl::pipeline _fxaa_pipeline;
    };
}