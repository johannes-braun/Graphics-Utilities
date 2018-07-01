#pragma once

#include <gfx/file.hpp>

namespace gfx
{
    enum class shader_stage
    {
        vertex,
        geometry,
        fragment,
        tess_evaluation,
        tess_control,

        compute
    };

    class shader
    {
    public:
        shader(shader_stage stage, const std::filesystem::path& file);

    private:

    };
}