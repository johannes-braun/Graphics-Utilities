#include <executable.hpp>
#include <random>

void executable::init(gfx::context_options& opt)
{
    opt.window_title        = "[17] Instanced";
    opt.debug               = true;
    opt.framebuffer_samples = gfx::sample_count::x8;
    opt.graphics_api        = gfx::gapi::opengl;
	opt.max_framerate = 10000.f;
}

struct instance_component : gfx::ecs::component<instance_component>
{
    glm::vec3 albedo;
	int buffer_index;
};

struct transform_component : gfx::ecs::component<transform_component>
{
    gfx::transform value;
};

struct target_component : gfx::ecs::component<target_component>
{
    gfx::transform target;
};

struct speed_component : gfx::ecs::component<speed_component>
{
	float speed = 10.f;
};

class movement_system : public gfx::ecs::system
{
public:
    movement_system()
    {
        add_component_type(transform_component::id);
        add_component_type(target_component::id);
        add_component_type(speed_component::id);
    }

    void update(double delta, gfx::ecs::component_base** components) const override
    {
		constexpr float epsilon = 0.1f;

        auto& tf  = components[0]->as<transform_component>();
        auto& tgt = components[1]->as<target_component>();
        auto& sp  = components[2]->as<speed_component>();

        const auto difference = tgt.target.position - tf.value.position;
		const auto distance = length(difference);
		const auto direction = normalize(difference);

        if(distance < epsilon)
        {
			tgt.target.position = glm::vec3(dist(gen), dist(gen), dist(gen)) * 200.f;
        }
		else
		{
			const glm::vec3 real_speed = direction * delta * glm::min(sp.speed, 2.f + distance);
			tf.value.position += real_speed;
			tf.value.rotation = glm::quatLookAt(direction, glm::vec3(0, 1, 0));
		}
    }

private:
	mutable std::mt19937                          gen;
	mutable std::uniform_real_distribution<float> dist;
};

void executable::run()
{
    struct instance
    {
        glm::mat4 matrix;
        glm::vec3 albedo;
        float     x;
    };

	gfx::ecs::ecs ecs;

    movement_system movement;
	gfx::ecs::system_list movement_systems;
	movement_systems.add(movement);

    gfx::hbuffer<instance> instances;

    std::mt19937                          gen;
    std::uniform_real_distribution<float> dist;
	std::vector<gfx::ecs::unique_entity> entities;
	const auto spawn_random = [&]() {
		instance_component inst;
		inst.albedo = glm::vec3(dist(gen), dist(gen), dist(gen));
		inst.buffer_index = entities.size();
		transform_component tf;
		tf.value = gfx::transform(glm::vec3(dist(gen), dist(gen), dist(gen)) * 50.f);
		target_component tgt;
		tgt.target = tf.value;
		speed_component sp;
		sp.speed = 8.f + 20.f * dist(gen);
		entities.push_back(ecs.create_entity_unique());
		entities.back()->add(inst);
		entities.back()->add(tf);
		entities.back()->add(tgt);
		entities.back()->add(sp);
    };
    for (int i = 0; i < 200; ++i) {
        spawn_random();
    }

    gfx::scene_file base_mesh("plane.dae");
    base_mesh.mesh.collapse();

    gfx::buffer<gfx::vertex3d> vbo(gfx::buffer_usage::vertex, base_mesh.mesh.vertices);
    gfx::buffer<gfx::index32>  ibo(gfx::buffer_usage::index, base_mesh.mesh.indices);

    gfx::pipe_state::vertex_input mesh_input;
    mesh_input.attributes.emplace_back(0, 0, gfx::rgb32f, offsetof(gfx::vertex3d, position));
    mesh_input.attributes.emplace_back(1, 0, gfx::rg32f, offsetof(gfx::vertex3d, uv));
    mesh_input.attributes.emplace_back(2, 0, gfx::rgb32f, offsetof(gfx::vertex3d, normal));
    mesh_input.bindings.emplace_back(0, sizeof(gfx::vertex3d));

    gfx::binding_layout mesh_bindings;
    mesh_bindings.push(gfx::binding_type::uniform_buffer).push(gfx::binding_type::storage_buffer);
    gfx::pipe_state::binding_layouts layouts;
    layouts.layouts.push_back(&mesh_bindings);
    gfx::pipe_state::depth_stencil depth_stencil;
    depth_stencil.depth_test_enable = true;
    gfx::pipe_state::rasterizer raster;
    raster.cull = gfx::cull_mode::back;
    gfx::pipe_state mesh_state;
    mesh_state.state_bindings      = &layouts;
    mesh_state.state_depth_stencil = &depth_stencil;
    mesh_state.state_multisample   = &msaa_state;
    mesh_state.state_rasterizer    = &raster;
    mesh_state.state_vertex_input  = &mesh_input;

    const auto shaders = {
        gfx::shader(gfx::shader_type::vert, "17_instanced/mesh.vert"),
        gfx::shader(gfx::shader_type::frag, "17_instanced/mesh.frag"),
    };
    gfx::graphics_pipeline mesh_pipeline(mesh_state, pass_layout, shaders);

    gfx::binding_set mesh_set(mesh_bindings);
    mesh_set.bind(0, *camera_buffer);
    mesh_set.bind(1, instances);

    while (frame()) {
        ImGui::Begin("Settings");
        static int spawn_count = 1;
        ImGui::DragInt("Spawn Count", &spawn_count, 0.05f, 0, 1000);
        if (ImGui::Button("Spawn Random")) {
            for (int i = 0; i < spawn_count; ++i) spawn_random();
            mesh_set.bind(1, instances);
        }
        ImGui::Value("Instances", int(instances.size()));
        ImGui::Value("Polygons", int(instances.size() * base_mesh.mesh.indices.size() / 3));
        ImGui::Value("Framerate", 1.f / float(context->delta()));
        ImGui::End();

		ecs.update(context->delta(), movement_systems);
		instances.resize(entities.size());
#pragma omp parallel for
        for(int i=0; i<entities.size(); ++i)
		{
			auto& e = entities[i];
			instances[i] = instance{ static_cast<glm::mat4>(e->get<transform_component>()->value), glm::vec4(e->get<instance_component>()->albedo, 1) };
		}
		mesh_set.bind(1, instances);

        current_command->begin_pass(*current_framebuffer);
        current_command->bind_pipeline(mesh_pipeline, {&mesh_set});
        current_command->bind_vertex_buffer(vbo, 0);
        current_command->bind_index_buffer(ibo, gfx::index_type::uint32);
        current_command->draw_indexed(ibo.capacity(), instances.size());
        current_command->end_pass();
    }
}