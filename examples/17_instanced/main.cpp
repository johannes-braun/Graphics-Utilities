#include <executable.hpp>
#include <gfx/file/json.hpp>
#include <random>

void executable::init(gfx::context_options& opt)
{
    opt.window_title        = "[17] Instanced";
    opt.debug               = false;
    opt.framebuffer_samples = gfx::sample_count::x8;
    opt.graphics_api        = gfx::gapi::vulkan;
    opt.max_framerate       = 10000.f;
}

struct instance_proto : gfx::drawcmd_indexed
{
    instance_proto(const gfx::scene_file& file, const gfx::image_file& logo)
          : gfx::drawcmd_indexed(file.mesh.indices.size())
          , vertex_count(file.mesh.vertices.size())
          , transform(glm::rotate(glm::radians(90.f), glm::vec3(1, 0, 0)))
          , logo(gfx::himage(gfx::rgba8unorm, logo))
          , logo_view(this->logo.view(gfx::imgv_type::image2d))
    {}

    std::string     name;
    float           speed          = 1.f;
    float           rotation_speed = 1.f;
    float           acceleration   = 1.f;
    gfx::u32        vertex_count   = 0;
    glm::mat4       transform;
    gfx::bounds3f   bounds;
    gfx::image      logo;
    gfx::image_view logo_view;
	gfx::u32 usages = 0;
};

struct instance : gfx::drawcmd_indexed
{
    alignas(16) glm::mat4 matrix;
    alignas(16) glm::vec3 albedo;
    alignas(16) gfx::bounds3f bounds;

    instance& operator=(const instance_proto& prototype)
    {
        index_count    = prototype.index_count;
        instance_count = prototype.instance_count;
        base_index     = prototype.base_index;
        base_vertex    = prototype.base_vertex;
        base_instance  = prototype.base_instance;
		bounds          = prototype.bounds;
        return *this;
    }
};

struct instance_component : gfx::ecs::component<instance_component>
{
    glm::vec3             albedo;
    int                   buffer_index;
    const instance_proto* prototype;
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
    glm::vec3 velocity{0, 0, 0};
    glm::vec3 acceleration{0, 0, 0};

    glm::vec3 angular_velocity{0, 0, 0};
    glm::vec3 angular_acceleration{0, 0, 0};
};

struct sphere_collider_component : gfx::ecs::component<sphere_collider_component>
{
	float radius = 1.f;
	float bounciness = 0.4f;
};

struct camera_component : gfx::ecs::component<camera_component>
{
	gfx::projection projection{glm::radians(80.f), 100, 100, 0.1f, 1000.f, true, true};
	bool is_main = true;
};

class movement_system : public gfx::ecs::system
{
public:
    movement_system()
    {
        add_component_type(transform_component::id);
        add_component_type(speed_component::id);
    }

    void update(double delta, gfx::ecs::component_base** components) const override
    {
        constexpr float movement_drag = 0.001f;
        constexpr float rotation_drag = 0.001f;

        auto& tf = components[0]->as<transform_component>();
        auto& sp = components[1]->as<speed_component>();

        sp.velocity = (1 - movement_drag) * sp.velocity + sp.acceleration * delta;
        tf.value.position += sp.velocity * delta;

        sp.angular_velocity = glm::mod((1 - rotation_drag) * sp.angular_velocity + sp.angular_acceleration * delta, glm::radians(360.f));
        tf.value.rotation *= glm::quat(sp.angular_velocity * delta);
    }

private:
    mutable std::mt19937                          gen;
    mutable std::uniform_real_distribution<float> dist;
};

const float chunk_size = 16.f;
const float chunk_count = 200;
class terrain
{
public:
	terrain(const std::filesystem::path& path)
		: _heightmap_local(gfx::r8unorm, path), _heightmap(_heightmap_local), _heightmap_view(gfx::imgv_type::image2d, _heightmap)
	{
		_sampler.set_wrap(gfx::wrap::u, gfx::wrap_mode::mirror_repeat);
		_sampler.set_wrap(gfx::wrap::v, gfx::wrap_mode::mirror_repeat);
	}

	const auto terrain_height(glm::vec2 xz) {
		xz += glm::vec2(chunk_size * chunk_count) / 2.f;
		xz /= chunk_size * chunk_count;
		return 220 * _heightmap_local.load_bilinear(glm::vec3(xz * glm::vec2(_heightmap_local.extents().vec), 0)).r;
	};

	const gfx::image_view& map_view() const noexcept { return _heightmap_view; }
	const gfx::sampler& sampler() const noexcept { return _sampler; }


private:
	gfx::himage        _heightmap_local;
	gfx::image         _heightmap;
	gfx::image_view         _heightmap_view;
	gfx::sampler _sampler;
};

class flying_in_circles_system : public gfx::ecs::system
{
public:
    flying_in_circles_system(terrain& t)
		: _terrain(t)
    {
        add_component_type(transform_component::id);
        add_component_type(speed_component::id);
        add_component_type(target_component::id);
        add_component_type(instance_component::id);
    }

    void update(double delta, gfx::ecs::component_base** components) const override
    {
        auto& tf = components[0]->as<transform_component>();
        auto& sp = components[1]->as<speed_component>();
        auto& tg = components[2]->as<target_component>();
        auto& in = components[3]->as<instance_component>();

        const auto fwd = tf.value.forward();
        const auto tgt = tg.target.position;
        const auto dir = normalize(tgt - tf.value.position);

		if (distance(tgt, tf.value.position) < 10.f)
		{
			const glm::vec2 p (1000.f * dist(gen) - 500.f, 1000.f * dist(gen) - 500.f);
			tg.target.position ={ p.x, _terrain.terrain_height(p) + 10.f, p.y };
		}
        else
        {
            tf.value.rotation =
                glm::slerp<float>(tf.value.rotation, glm::quatLookAt(dir, glm::vec3(0, 1, 0)), in.prototype->rotation_speed * delta);
            sp.acceleration = in.prototype->acceleration * fwd;
            if (length(sp.velocity) > in.prototype->speed) sp.velocity = normalize(sp.velocity) * in.prototype->speed;
            sp.acceleration -= 200.f * (-(dot(tf.value.left(), dir))) * tf.value.left() * delta * length(sp.velocity);
            sp.acceleration -= 200.f * (-(dot(tf.value.up(), dir))) * tf.value.up() * delta * length(sp.velocity);
        }
    }

private:
	terrain& _terrain;
    mutable std::mt19937                          gen;
    mutable std::uniform_real_distribution<float> dist;
};

class prototype_renderer
{
public:
    prototype_renderer()
    {
        std::ifstream  protos_file(gfx::file("protos.json"));
        nlohmann::json protos;
        protos_file >> protos;
        std::ifstream  buildings_file(gfx::file("buildings.json"));
        nlohmann::json buildings;
        buildings_file >> buildings;

        gfx::mesh3d combined;
        for (const auto& json : protos) {
            const auto name           = json["name"].get<std::string>();
            const auto path           = json["mesh"].get<std::string>();
            const auto speed          = json["speed"].get<float>();
            const auto acceleration   = json["acceleration"].get<float>();
            const auto rotation_speed = json["rotation_speed"].get<float>();
			const auto logo = json["logo"].get<std::string>();

            gfx::scene_file mesh(path);
            mesh.mesh.collapse();
            instance_proto& proto = _vehicle_prototypes.emplace_back(mesh, gfx::image_file(logo, gfx::bits::b8, 4));
            proto.name            = name;
            proto.speed           = speed;
            proto.acceleration    = acceleration;
            proto.rotation_speed  = rotation_speed;
            proto.base_vertex     = combined.vertices.size();
            proto.base_index      = combined.indices.size();
            proto.bounds          = mesh.mesh.compute_bounds();
            combined += mesh.mesh;
        }
		for (const auto& json : buildings) {
			const auto name = json["name"].get<std::string>();
			const auto path = json["mesh"].get<std::string>();
			const auto logo = json["logo"].get<std::string>();

			gfx::scene_file mesh(path);
			mesh.mesh.collapse();
			instance_proto& proto = _building_prototypes.emplace_back(mesh, gfx::image_file(logo, gfx::bits::b8, 4));
			proto.name            = name;
			proto.speed           = 0;
			proto.base_vertex     = combined.vertices.size();
			proto.base_index      = combined.indices.size();
			proto.bounds          = mesh.mesh.compute_bounds();
			combined += mesh.mesh;
		}

        _vbo = gfx::buffer<gfx::vertex3d>(gfx::buffer_usage::vertex, combined.vertices);
        _ibo = gfx::buffer<gfx::index32>(gfx::buffer_usage::index, combined.indices);
    }

    void submit(const instance& i) { _instances.push_back(i); }

    void render(gfx::commands& cmd) const
    {
        cmd.bind_vertex_buffer(_vbo, 0);
        cmd.bind_index_buffer(_ibo, gfx::index_type::uint32);
        cmd.draw_indirect_indexed(_instances, _instances.size());
    }

    void clear() { _instances.clear(); }

    const instance_proto* by_name(const std::string& name) const noexcept
    {
        if (const auto it =
                std::find_if(_vehicle_prototypes.begin(), _vehicle_prototypes.end(), [&](const auto& proto) { return proto.name == name; });
            it != _vehicle_prototypes.end())
        {
            return &*it;
        }
        if (const auto it = std::find_if(_building_prototypes.begin(), _building_prototypes.end(),
                                         [&](const auto& proto) { return proto.name == name; });
            it != _building_prototypes.end())
        {
            return &*it;
        }
        return nullptr;
    }
    const std::vector<instance_proto>& vehicle_prototypes() const noexcept { return _vehicle_prototypes; }
    const std::vector<instance_proto>& building_prototypes() const noexcept { return _building_prototypes; }
    const gfx::hbuffer<instance>&      instances() const noexcept { return _instances; };

private:
    std::vector<instance_proto> _vehicle_prototypes;
    std::vector<instance_proto> _building_prototypes;
    gfx::buffer<gfx::vertex3d>  _vbo{gfx::buffer_usage::vertex};
    gfx::buffer<gfx::index32>   _ibo{gfx::buffer_usage::index};
    gfx::hbuffer<instance>      _instances;
};

class prototype_system : public gfx::ecs::system
{
public:
    prototype_system(prototype_renderer& r) : _renderer(r)
    {
        add_component_type(instance_component::id);
        add_component_type(transform_component::id);
		add_component_type(camera_component::id, gfx::ecs::component_flag::optional);
    }

    void update(double delta, gfx::ecs::component_base** components) const override
    {
        const auto& ic = components[0]->as<instance_component>();
        const auto& tf = components[1]->as<transform_component>();

		if (components[2])
			return;

        instance i;
        i        = *ic.prototype;
        i.albedo = glm::vec4(ic.albedo, 1);
        i.matrix = tf.value * ic.prototype->transform;
        _renderer.submit(i);
    }

private:
    prototype_renderer& _renderer;
};

class camera_system : public gfx::ecs::system
{
public:
	camera_system()
	{
		add_component_type(camera_component::id);
		add_component_type(transform_component::id);
	}

	void update(double delta, gfx::ecs::component_base** components) const override
	{
		auto& cam = components[0]->as<camera_component>();
		const auto& tf = components[1]->as<transform_component>();

		int w, h;
		glfwGetFramebufferSize(gfx::context::current()->window(), &w, &h);

		cam.projection.perspective().clip_near = 0.01f;
		cam.projection.perspective().clip_far = 2500.f;
		cam.projection.perspective().screen_width = w;
		cam.projection.perspective().screen_height = h;
		_camera.projection_mode = cam.projection;
		_camera.transform_mode = tf.value;
		_main_component = &cam;
	}

	const gfx::camera& camera() const noexcept { return _camera; }
	camera_component* cam_component() const noexcept { return _main_component; }

private:
	mutable camera_component* _main_component;
	mutable gfx::camera _camera;
};

class collision_resolver
{
public:
	collision_resolver(terrain& t)
		: _terrain(t)
	{

	}

	void clear()
	{
		_colliders.clear();
	}

	void resolve(transform_component& tc, speed_component& sc, sphere_collider_component& scc)
	{
		const auto th = _terrain.terrain_height(glm::vec2(tc.value.position.x, tc.value.position.z));
		if (tc.value.position.y - scc.radius < th)
		{
			tc.value.position.y = th + scc.radius;
			const float eps = 1.f;
			const auto get_position = [&] (const glm::vec2& v) {
				return glm::vec3(v.x, _terrain.terrain_height(v), v.y);
			};
			const glm::vec3 dp0 = get_position(glm::vec2(-eps, -eps));
			const glm::vec3 dp1 = get_position(glm::vec2(-eps, eps));
			const glm::vec3 dp2 = get_position(glm::vec2(eps, eps));
			const glm::vec3 dp3 = get_position(glm::vec2(eps, -eps));
			const glm::vec3 normal_0 = normalize(cross(dp2 - dp0, dp3 - dp1));

			const glm::vec3 ref = glm::reflect(sc.velocity, normal_0);
			sc.velocity = scc.bounciness * ref + 8.f * normal_0; 
		}

#pragma omp parallel for
		for (int i=0; i<_colliders.size(); ++i)
		{
			auto& t_tc = *std::get<transform_component*>(_colliders[i]);
			auto& t_sc = *std::get<speed_component*>(_colliders[i]);
			auto& t_scc = *std::get<sphere_collider_component*>(_colliders[i]);

			if (distance(tc.value.position, t_tc.value.position) < (scc.radius + t_scc.radius))
			{
				const glm::vec3 normal = normalize(t_tc.value.position - tc.value.position);
				const glm::vec3 ref = glm::reflect(sc.velocity, normal);
				const glm::vec3 t_ref = glm::reflect(t_sc.velocity, -normal);

				sc.velocity = scc.bounciness * ref + 0.1f * normal; 
				t_sc.velocity = t_scc.bounciness * t_ref - 0.1f * normal;

				tc.value.position = t_tc.value.position - (scc.radius + t_scc.radius) * normal;
			}
		}
		_colliders.emplace_back(&tc, &sc, &scc);
	}

private:
	std::vector<std::tuple<transform_component*, speed_component*, sphere_collider_component*>> _colliders;
	terrain& _terrain;
};

class collision_system : public gfx::ecs::system
{
public:
	collision_system(collision_resolver& resolver)
		: _resolver(resolver)
	{
		add_component_type(transform_component::id);
		add_component_type(speed_component::id);
		add_component_type(sphere_collider_component::id);
	}

	void update(double delta, gfx::ecs::component_base** components) const override
	{
		auto& tc = components[0]->as<transform_component>();
		auto& sc = components[1]->as<speed_component>();
		auto& scc = components[2]->as<sphere_collider_component>();
		_resolver.resolve(tc, sc, scc);
	}

private:
	collision_resolver& _resolver;
};

void executable::run()
{
    gfx::ecs::ecs ecs;
	camera.projection_mode.perspective().clip_near = 0.01f;
	camera.projection_mode.perspective().clip_far = 2500.f;

	terrain main_terrain("heightmap.png");
	collision_resolver coll_resolver(main_terrain);

    movement_system          movement;
    flying_in_circles_system circle_flyer(main_terrain);
	collision_system coll_system(coll_resolver);
    gfx::ecs::system_list    movement_systems;
    movement_systems.add(movement);
    movement_systems.add(circle_flyer);
    movement_systems.add(coll_system);

    prototype_renderer    renderer;
    gfx::ecs::system_list graphics_systems;
    prototype_system      proto_system(renderer);
	camera_system		  cam_system;
    graphics_systems.add(proto_system);
    graphics_systems.add(cam_system);

	gfx::sampler sampler;
	sampler.set_wrap(gfx::wrap::u, gfx::wrap_mode::repeat);
	sampler.set_wrap(gfx::wrap::v, gfx::wrap_mode::repeat);
    std::mt19937                          gen;
    std::uniform_real_distribution<float> dist;
    std::vector<gfx::ecs::entity>  entities;
    const auto                            spawn_from_prototype = [&](const instance_proto* prototype, const glm::vec3 at) {
        instance_component inst;
        inst.albedo       = glm::vec3(dist(gen), dist(gen), dist(gen));
        inst.buffer_index = entities.size();
        inst.prototype    = prototype;
		const_cast<instance_proto*>(prototype)->usages++;
        transform_component tf;
        tf.value = gfx::transform(at);
        target_component tgt;
        tgt.target = tf.value;
        speed_component sp;
		sphere_collider_component collider;
		collider.radius = 2.f;
		entities.push_back(ecs.create_entity(inst, tf, tgt, sp, collider));
		return entities.back();
    };
    const auto spawn_random_vehicle = [&]() {
        return spawn_from_prototype(&renderer.vehicle_prototypes()[std::clamp(size_t(dist(gen) * renderer.vehicle_prototypes().size()), 0ull,
                                                                       renderer.vehicle_prototypes().size())],
                             glm::vec3(dist(gen), dist(gen), dist(gen)) * 50.f);
    };

    for (int i = 0; i < 500; ++i) {
		if (i == 0)
			spawn_random_vehicle().add(camera_component{});
		else
			spawn_random_vehicle();
    }

   const auto spawn_random_building = [&]() {
        instance_component inst;
        inst.albedo       = glm::vec3(dist(gen), dist(gen), dist(gen));
        inst.buffer_index = entities.size();
        inst.prototype    = &renderer.building_prototypes()[std::clamp(size_t(dist(gen) * renderer.building_prototypes().size()), 0ull,
                                                                    renderer.building_prototypes().size())];
		const_cast<instance_proto*>(inst.prototype)->usages++;
        transform_component tf;
        glm::vec2           loc = 1000.f * glm::vec2(dist(gen), dist(gen));
        tf.value                = gfx::transform(glm::vec3(loc.x, 5.f + main_terrain.terrain_height(loc), loc.y));
        entities.push_back(ecs.create_entity(inst, tf));
    };
    for (int i = 0; i < 50; ++i) {
        spawn_random_building();
    }


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
    const auto shaders             = {
        gfx::shader(gfx::shader_type::vert, "17_instanced/mesh.vert"),
        gfx::shader(gfx::shader_type::frag, "17_instanced/mesh.frag"),
    };
    gfx::graphics_pipeline mesh_pipeline(mesh_state, pass_layout, shaders);

    gfx::compute_pipeline cull_pipeline(layouts, gfx::shader(gfx::shader_type::comp, "17_instanced/cull.comp"));

    struct terrain_chunk
    {
        glm::vec2 position;
    };

    gfx::buffer<glm::vec2>      chunk(gfx::buffer_usage::vertex, {
                                                                chunk_size * glm::vec2(0, 0),
                                                                chunk_size * glm::vec2(0, 1),
                                                                chunk_size * glm::vec2(1, 1),
                                                                chunk_size * glm::vec2(1, 0),
                                                            });
    gfx::hbuffer<terrain_chunk> chunks(chunk_count * chunk_count);

    for (int x = 0; x < chunk_count; ++x) {
        for (int y = 0; y < chunk_count; ++y) {
            chunks[x + chunk_count * y].position =
                glm::vec2((x - (chunk_count / 2.f)) * chunk_size, (y - (chunk_count / 2.f)) * chunk_size);
        }
    }

    gfx::binding_layout terrain_bindings;
    terrain_bindings.push(gfx::binding_type::uniform_buffer).push(gfx::binding_type::sampled_image);
	gfx::binding_layout terrain_style_bindings;
	terrain_style_bindings.push(gfx::binding_type::sampled_image).push(gfx::binding_type::sampled_image);
    gfx::pipe_state::binding_layouts terrain_layouts;
    terrain_layouts.layouts.push_back(&terrain_bindings);
    terrain_layouts.layouts.push_back(&terrain_style_bindings);
    gfx::pipe_state::vertex_input terrain_input;
    terrain_input.attributes.emplace_back(0, 0, gfx::rg32f, 0);
    terrain_input.attributes.emplace_back(1, 1, gfx::rg32f, offsetof(terrain_chunk, position));
    terrain_input.bindings.emplace_back(0, sizeof(glm::vec2), gfx::input_rate::vertex);
    terrain_input.bindings.emplace_back(1, sizeof(terrain_chunk), gfx::input_rate::instance);
    gfx::pipe_state::tesselation terrain_tess;
    terrain_tess.patch_control_points = 4;
    gfx::pipe_state::input_assembly terrain_assembly;
    terrain_assembly.primitive_topology = gfx::topology::patch_list;
    gfx::pipe_state::rasterizer terrain_rasterizer;
    terrain_rasterizer.polygon_mode = gfx::poly_mode::fill;
    gfx::pipe_state terrain_state;
    terrain_state.state_rasterizer     = &terrain_rasterizer;
    terrain_state.state_vertex_input   = &terrain_input;
    terrain_state.state_input_assembly = &terrain_assembly;
    terrain_state.state_bindings       = &terrain_layouts;
    terrain_state.state_tesselation    = &terrain_tess;
    terrain_state.state_multisample    = &msaa_state;
    const auto terrain_shaders         = {
        gfx::shader(gfx::shader_type::vert, "17_instanced/terrain.vert"),
        gfx::shader(gfx::shader_type::tesc, "17_instanced/terrain.tesc"),
        gfx::shader(gfx::shader_type::tese, "17_instanced/terrain.tese"),
        gfx::shader(gfx::shader_type::frag, "17_instanced/terrain.frag"),
    };
    gfx::graphics_pipeline terrain_pipeline(terrain_state, pass_layout, terrain_shaders);


	gfx::hbuffer<float> time_buffer{ 2.f };
	gfx::binding_layout sky_bindings;
	sky_bindings.push(gfx::binding_type::uniform_buffer).push(gfx::binding_type::uniform_buffer);
	gfx::pipe_state::binding_layouts sky_binding_state;
	sky_binding_state.layouts.push_back(&sky_bindings);
	gfx::binding_set sky_set(sky_bindings);
	sky_set.bind(0, *camera_buffer);
	sky_set.bind(1, time_buffer);
	gfx::pipe_state::depth_stencil sky_depth;
	sky_depth.depth_write_enable = false;
	sky_depth.depth_test_enable  = true;
	gfx::pipe_state sky_state;
	sky_state.state_bindings      = &sky_binding_state;
	sky_state.state_depth_stencil = &sky_depth;
	sky_state.state_multisample   = &msaa_state;
	gfx::graphics_pipeline sky_pipeline(sky_state, pass_layout,
		{gfx::shader(gfx::shader_type::vert, "14_cubemapper/skybox.vert"),
		gfx::shader(gfx::shader_type::frag, "14_cubemapper/skybox.frag")});


	gfx::image& terrain_bump = res.images_ldr["Rock_023_DISP.png"];
	gfx::image_view terrain_bump_view = terrain_bump.view(gfx::imgv_type::image2d);
	gfx::image& terrain_color = res.images_ldr["Rock_023_COLOR.jpg"];
	gfx::image_view terrain_color_view = terrain_color.view(gfx::imgv_type::image2d);

    gfx::binding_set terrain_set(terrain_bindings);
    terrain_set.bind(0, *camera_buffer);
    terrain_set.bind(1, main_terrain.map_view(), main_terrain.sampler());
	gfx::binding_set terrain_style_set(terrain_style_bindings);
	terrain_style_set.bind(0, terrain_bump_view, sampler);
	terrain_style_set.bind(1, terrain_color_view, sampler);

    std::vector<gfx::binding_set> mesh_sets;
    mesh_sets.reserve(context->swapchain()->image_views().size());
    for (int i = 0; i < context->swapchain()->image_views().size(); ++i) {
        mesh_sets.emplace_back(mesh_bindings);
        mesh_sets.back().bind(0, *camera_buffer);
    }

	int fc = 0;
	double t = 0;
	double ftime = 1.f;
	while (frame()) {
		++fc;
		t += context->delta();

		if (t > 1)
		{
			ftime = t / fc;
			t = 0.0;
			fc = 0;
		}

		static int spawn_count = 1;
		static bool enable_following_camera = false;

        ImGui::Begin("Settings");
        ImGui::DragInt("Spawn Count", &spawn_count, 0.05f, 0, 1000);
        if (ImGui::Button("Spawn Random")) {
            for (int i = 0; i < spawn_count; ++i) spawn_random_vehicle();
        }
        ImGui::Value("Instances", int(renderer.instances().size()));
        ImGui::Value("Framerate", 1.f / float(ftime));
		ImGui::Checkbox("Follow any", &enable_following_camera);
		if (enable_following_camera && ImGui::Button("Random cam"))
		{
			gfx::dlog << ecs.remove_components<camera_component>(cam_system.cam_component()->entity);
			entities[int(dist(gen) * 500)].add(camera_component{});
		}
		ImGui::Separator();

        for(auto& proto : renderer.vehicle_prototypes())
        {
			ImGui::PushID(proto.name.c_str());
			ImGui::Image(&proto.logo_view, ImVec2(64, 64));
			ImGui::SameLine();
			ImGui::Text(proto.name.c_str());
			ImGui::SameLine();
			ImGui::Value("usg: ", proto.usages);
			ImGui::SameLine();
            if(ImGui::Button("Spawn"))
            {
				spawn_from_prototype(&proto, camera.transform_mode.position + camera.transform_mode.forward() * 8.f);
            }
			ImGui::PopID();
        }

        ImGui::End();

		/*camera.transform_mode.position.y = terrain_height({ camera.transform_mode.position.x, camera.transform_mode.position.z }) + 1.8f;*/

        renderer.clear();
		coll_resolver.clear();
		ecs.update(context->delta(), movement_systems);
        ecs.update(context->delta(), graphics_systems);
        mesh_sets[context->swapchain()->current_image()].bind(1, renderer.instances());

		if(enable_following_camera)
			current_command->update_buffer(*camera_buffer, 0, cam_system.camera().info());
		
        current_command->bind_pipeline(cull_pipeline, {&mesh_sets[context->swapchain()->current_image()]});
        current_command->dispatch((renderer.instances().size() + 31) / 32);

        current_command->begin_pass(*current_framebuffer);
		current_command->bind_pipeline(sky_pipeline, { &sky_set });
		current_command->draw(3);

        current_command->bind_pipeline(mesh_pipeline, {&mesh_sets[context->swapchain()->current_image()]});
        current_command->render(renderer);

        current_command->bind_pipeline(terrain_pipeline, {&terrain_set, &terrain_style_set});
        current_command->bind_vertex_buffer(chunk, 0);
        current_command->bind_vertex_buffer(chunks, 1);
        current_command->draw({gfx::u32(chunk.size()), gfx::u32(chunks.size())});

        current_command->end_pass();
    }
}