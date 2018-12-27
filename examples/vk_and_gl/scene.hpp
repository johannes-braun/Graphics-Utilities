#pragma once

#include "gfx.file/file.hpp"

namespace scene
{
    inline const gfx::scene_file& current_scene()
    {
        static gfx::scene_file scene("sponza/sponza.obj", 0.01f);
        return scene;
    }

}