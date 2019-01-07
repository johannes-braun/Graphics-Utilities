#pragma once

#include "camera.hpp"
#include "gfx.file/file.hpp"
#include "shaders/def.hpp"
#include <gfx.ecs.defaults2/prototype.hpp>
#include <random>
#include <shared_mutex>

namespace scene {

struct scene_manager_t
{
    ~scene_manager_t()
    {
        if (_loader_thread.joinable()) _loader_thread.join();
    }

    void load(const std::filesystem::path& path, float scale, bool make_current = true, const std::function<bool(float)>& on_progress = {})
    {
        const auto cancelable_progress_fun = [=](float p) {
            if (!_cancel_load) return on_progress(p);
            return false;
        };
        if (_loader_thread.joinable())
        {
            _cancel_load = true;
            _loader_thread.join();
        }
        _cancel_load = false;

        _loader_thread = std::thread{[=] {
            const size_t     hash = std::hash<std::string>()(path.string()) ^ std::hash<float>()(scale);
            std::unique_lock lock(_scene_mutex);

            const gfx::scene_file* fptr = [&] {
                if (const auto it = _loaded_scenes.find(hash); it != _loaded_scenes.end())
                    return ((void)cancelable_progress_fun(1.f), &it->second);
                return &(_loaded_scenes.emplace(hash, gfx::scene_file(path, scale, cancelable_progress_fun)).first->second);
            }();
            if (make_current) { _current_scene = fptr; }
        }};
    }

    const gfx::scene_file& current() const
    {
        assert(_current_scene.load());
        std::shared_lock lock(_scene_mutex);
        return *_current_scene.load();
    }

    static std::vector<gfx::ecs::shared_entity> get_mesh_entities(gfx::ecs::ecs& ecs, gfx::instance_system<def::mesh_info>& instances,
                                                                  const gfx::scene_file&                       scene,
                                                                  std::function<int(const gfx::image_file& t)> make_texture_id)
    {
        std::vector<gfx::ecs::shared_entity> mesh_entities;
        std::vector<def::mesh_info>          mesh_infos;
        for (size_t i = 0; i < scene.materials.size(); ++i)
        {
            def::mesh_info& info = mesh_infos.emplace_back();
            info.diffuse_color   = 255 * clamp(scene.materials[i].color_diffuse, 0.f, 1.f);

            info.diffuse_texture_id = -1;
            if (scene.materials[i].texture_diffuse.bytes())
            { info.diffuse_texture_id = make_texture_id(scene.materials[i].texture_diffuse); } if (scene.materials[i].texture_bump.bytes())
            { info.bump_map_texture_id = make_texture_id(scene.materials[i].texture_bump); } }

        instances.get_mesh_allocator().reserve_for(scene.mesh.vertices.size(), scene.mesh.indices.size());
        for (size_t i = 0; i < scene.mesh.geometries.size(); ++i)
        {
            gfx::shared_mesh      mesh  = instances.get_mesh_allocator().allocate_mesh(scene.mesh, scene.mesh.geometries[i], true);
            gfx::shared_prototype proto = instances.get_instantiator().allocate_prototype(scene.mesh_names[i], {&mesh, 1});

            gfx::instance_component<def::mesh_info> instance_component(std::move(proto), mesh_infos[scene.mesh_material_indices.at(i)]);
            gfx::transform_component                transform_component = scene.mesh.geometries[i].transformation.matrix();
            mesh_entities.emplace_back(ecs.create_entity_shared(instance_component, transform_component));
        }
        instances.get_mesh_allocator().commit();
        return mesh_entities;
    }

    std::atomic<const gfx::scene_file*>         _current_scene = nullptr;
    std::atomic_bool                            _cancel_load   = false;
    mutable std::shared_mutex                   _scene_mutex;
    std::unordered_map<size_t, gfx::scene_file> _loaded_scenes;
    std::thread                                 _loader_thread;
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