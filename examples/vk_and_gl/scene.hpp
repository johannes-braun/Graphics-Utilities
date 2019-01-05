#pragma once

#include "gfx.file/file.hpp"

namespace scene {
inline const gfx::scene_file& current_scene()
{
    static gfx::scene_file scene("sponza/sponza.obj", 0.01f);
    return scene;
}

inline struct interop_mgr_t
{
    gfx::ecs::entity user_entity_vulkan;
    gfx::ecs::entity user_entity_opengl;

    enum class match
    {
        vulkan_sided,
        opengl_sided
    };

    void match_cameras(match m)
    {
        gfx::camera_controls* tcv = user_entity_vulkan.get<gfx::camera_controls>();
        gfx::camera_controls* tcg = user_entity_opengl.get<gfx::camera_controls>();

        switch (m)
        {
        case match::vulkan_sided: tcg->target_transform = tcv->target_transform; break;
        case match::opengl_sided: tcv->target_transform = tcg->target_transform; break;
        default:;
        }
    }
} interop;

}    // namespace scene