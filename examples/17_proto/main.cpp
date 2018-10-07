#include <executable.hpp>
#include <gfx/file/json.hpp>
#include <random>

#include "csm.hpp"
#include "graphics.hpp"
#include "interaction.hpp"
#include "movement.hpp"
#include "prototype.hpp"
#include "prototype_system.hpp"
#include "terrain.hpp"

void executable::init(gfx::context_options& opt)
{
    opt.window_title        = "[17] Instanced";
    opt.debug               = true;
    opt.framebuffer_samples = gfx::sample_count::x4;
    opt.graphics_api        = gfx::gapi::vulkan;
    opt.max_framerate       = 10000;
}

void executable::run()
{
    std::mt19937                                gen;
    const std::uniform_real_distribution<float> dist;

    graphics core(*this);
    csm      shadow_map(core, 1024, 5);

    interaction_processor interaction_manager(ecs);

    user_entity->get<gfx::camera_component>()->projection.perspective().clip_near = 0.01f;
    user_entity->get<gfx::camera_component>()->projection.perspective().clip_far  = 400.f;
    std::vector<gfx::ecs::entity> entities;

    terrain main_terrain("heightmap.png", 2.f, 400);

    movement_system       movement;
    gfx::ecs::system_list movement_systems;
    movement_systems.add(movement);

    prototype_manager prototypes;
    prototype_system  proto_system(prototypes);

    gfx::scene_file tplane("proto/techplane.dae");
    tplane.mesh.collapse();
    unique_mesh      um    = prototypes.allocate_mesh_unique(tplane.mesh.vertices, tplane.mesh.indices);
    std::array       s     = {um.get()};
    unique_prototype proto = prototypes.allocate_prototype_unique("Tech Plane", s);

    for (auto i = 0ull; i < 1200; ++i)
    {
        prototype_component tplanecomp;
        tplanecomp.proto   = proto.get();
        tplanecomp.visible = true;
        tplanecomp.color   = glm::vec4(dist(gen), dist(gen), dist(gen), 1.f);

        gfx::transform_component transform;
        transform.value.position   = glm::vec3(200.f * dist(gen) - 100.f, 0.f, 200.f * dist(gen) - 100.f);
        transform.value.position.y = main_terrain.terrain_height({transform.value.position.x, transform.value.position.z}) + 10.f;
        ecs.create_entity(tplanecomp, transform);
    }


    gfx::ecs::system_list graphics_systems;
    graphics_systems.add(proto_system);

    const gfx::hbuffer<float> time_buffer{2.f};

    gfx::image&           terrain_bump       = res.images_ldr["6675-bump.jpg"];
    const gfx::image_view terrain_bump_view  = terrain_bump.view(gfx::imgv_type::image2d);
    gfx::image&           terrain_color      = res.images_ldr["14063348-rocks-and-gravel-seamless-texture-tile.jpg"];
    const gfx::image_view terrain_color_view = terrain_color.view(gfx::imgv_type::image2d);
    gfx::image&           mesh_color         = res.images_ldr["rust.png"];
    const gfx::image_view mesh_color_view    = mesh_color.view(gfx::imgv_type::image2d);

	gfx::binding_set sky_set(core.sky_bindings);
	sky_set.bind(0, *camera_buffer);
	sky_set.bind(1, time_buffer);

    gfx::binding_set main_camera_set(core.camera_bindings);
    main_camera_set.bind(0, *camera_buffer);

    gfx::binding_set terrain_info_set(core.terrain_info_bindings);
    terrain_info_set.bind(0, main_terrain.map_view(), main_terrain.sampler());
    terrain_info_set.bind(1, main_terrain.info());

    gfx::binding_set terrain_style_set(core.terrain_style_bindings);
    terrain_style_set.bind(0, terrain_bump_view, core.default_sampler);
    terrain_style_set.bind(1, terrain_color_view, core.default_sampler);

    gfx::binding_set mesh_style_set(core.mesh_style_bindings);
    mesh_style_set.bind(0, mesh_color_view, core.default_sampler);

    std::vector<gfx::binding_set> mesh_sets;
    mesh_sets.reserve(context->swapchain()->image_views().size());
    for (auto i = 0ull; i < context->swapchain()->image_views().size(); ++i) { mesh_sets.emplace_back(core.mesh_bindings); }

    int    fc    = 0;
    double t     = 0;
    double ftime = 1.f;
    while (frame())
    {
        ++fc;
        t += context->delta();

        if (t > 1)
        {
            ftime = t / fc;
            t     = 0.0;
            fc    = 0;
        }

        static int  spawn_count             = 1;
        static bool enable_following_camera = false;

        ImGui::Begin("Settings");
        ImGui::DragInt("Spawn Count", &spawn_count, 0.05f, 0, 1000);
        ImGui::Value("Framerate", 1.f / float(ftime));
        ImGui::Value("Frametime (ms)", float(ftime * 1000.0));
        ImGui::Checkbox("Follow any", &enable_following_camera);
        if (enable_following_camera && ImGui::Button("Random cam"))
        { entities[int(dist(gen) * entities.size())].add(gfx::camera_component{}); } ImGui::Separator();

        static bool show_light_cam = false;
        ImGui::Checkbox("Show Light Camera", &show_light_cam);

        ImGui::End();

        prototypes.clear_commands();
        ecs.update(context->delta(), movement_systems);
        ecs.update(context->delta(), graphics_systems);

        mesh_sets[context->swapchain()->current_image()].bind(0, prototypes.current_commands());

        if (enable_following_camera) { current_command->update_buffer(*camera_buffer, 0, *gfx::get_camera_info(ecs, *user_entity)); }

        glm::vec3 campos = user_entity->get<gfx::transform_component>()->value.position;
        campos.y         = main_terrain.terrain_height({campos.x, campos.z});
        shadow_map.render(*current_command, campos,
                          [&](gfx::binding_set& light_camera) {
                              current_command->bind_pipeline(*core.mesh_cull_pipeline,
                                                             {&light_camera, &mesh_sets[context->swapchain()->current_image()]});
                              current_command->dispatch(gfx::u32((prototypes.current_commands().size() + 31) / 32));
                          },
                          [&](gfx::binding_set& light_camera) {
                              current_command->bind_pipeline(*core.mesh_shadow_pipeline,
                                                             {&light_camera, &mesh_sets[context->swapchain()->current_image()]});
                              current_command->render(prototypes);

                              current_command->bind_pipeline(*core.terrain_shadow_pipeline, {&terrain_info_set, &light_camera});
                              current_command->bind_vertex_buffer(main_terrain.chunk_buffer(), 0);
                              current_command->draw({gfx::u32(main_terrain.chunk_buffer().size())});
                          });

        current_command->bind_pipeline(*core.mesh_cull_pipeline, {&main_camera_set, &mesh_sets[context->swapchain()->current_image()]});
        current_command->dispatch(gfx::u32((prototypes.current_commands().size() + 31) / 32));
        current_command->begin_pass(*current_framebuffer);
        current_command->bind_pipeline(*core.sky_pipeline, {&sky_set});
        current_command->draw(3);
        current_command->bind_pipeline(*core.mesh_render_pipeline, {&main_camera_set, &mesh_sets[context->swapchain()->current_image()],
                                                                    &mesh_style_set, &shadow_map.shadow_set});
        current_command->render(prototypes);
        current_command->bind_pipeline(*core.terrain_render_pipeline,
                                       {&terrain_info_set, &main_camera_set, &terrain_style_set, &shadow_map.shadow_set});
        current_command->bind_vertex_buffer(main_terrain.chunk_buffer(), 0);
        current_command->draw({gfx::u32(main_terrain.chunk_buffer().size())});
        current_command->end_pass();
    }
}