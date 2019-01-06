#pragma once

#include "camera.hpp"
#include "gfx.file/file.hpp"

namespace scene {

struct scene_manager_t
{
    const gfx::scene_file& load(const std::filesystem::path& path, float scale, bool make_current = true)
    {
        const size_t     hash = std::hash<std::string>()(path.string()) ^ std::hash<float>()(scale);
        std::unique_lock lock(_scene_mutex);

        const gfx::scene_file* fptr = [&] {
            if (const auto it = _loaded_scenes.find(hash); it != _loaded_scenes.end()) return &it->second;
            return &(_loaded_scenes.emplace(hash, gfx::scene_file(path, scale)).first->second);
        }();
        if (make_current) { _current_scene = fptr; }
        return *fptr;
    }

    const gfx::scene_file& current() const
    {
        assert(_current_scene.load());
        return *_current_scene.load();
    }

    std::atomic<const gfx::scene_file*>         _current_scene = nullptr;
    std::mutex                                  _scene_mutex;
    std::unordered_map<size_t, gfx::scene_file> _loaded_scenes;
};
    
inline scene_manager_t scene_manager;

inline const gfx::scene_file& current_scene()
{
    return scene_manager.current();
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