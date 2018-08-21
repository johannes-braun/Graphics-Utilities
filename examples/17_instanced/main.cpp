#include <chrono>
#include <executable.hpp>
#include <gfx/file/json.hpp>
#include <random>

void executable::init(gfx::context_options& opt)
{
    opt.window_title        = "[17] Instanced";
    opt.debug               = true;
    opt.framebuffer_samples = gfx::sample_count::x4;
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
    gfx::u32        usages = 0;
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
        bounds         = prototype.bounds;
        return *this;
    }
};

struct instance_component : gfx::ecs::component<instance_component>
{
    glm::vec3             albedo;
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
    glm::vec3 impulse{0, 0, 0};
    glm::vec3 force{0, 0, 0};
    glm::vec3 rotation_impulse{0, 0, 0};
    glm::vec3 torque{0, 0, 0};
    glm::vec3 torque_acceleration{0, 0, 0};
};

struct sphere_collider_component : gfx::ecs::component<sphere_collider_component>
{
    float radius     = 0.3f;
    float bounciness = 0.8f;
};

struct camera_component : gfx::ecs::component<camera_component>
{
    gfx::projection projection{glm::radians(80.f), 100, 100, 0.1f, 1000.f, true, true};
    bool            is_main = true;
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
        auto& tf = components[0]->as<transform_component>();
        auto& sp = components[1]->as<speed_component>();

        const auto sphere_radius = 0.3f;
        const auto sphere_moment = 2.f / 5.f * sphere_radius * sphere_radius;
        const auto moment        = sphere_moment;

        const auto mass            = 10.f;
        const auto weighted_moment = mass * moment;


        /*	if ((type & RigidBodyTypeFlagBits::eDynamic) != RigidBodyTypeFlagBits::eDynamic)
                return;

            const auto dt = scene()->fixed_time_step;*/
        const float dt      = delta;
        const auto  gravity = glm::vec3(0, -9.81f, 0);

        sp.impulse = 0.999f * sp.impulse + (sp.force + gravity * mass) * dt;

        // if (isnan(sp.impulse.x)) gfx::elog << "nan";

        tf.value.position += sp.impulse / mass * dt;

        sp.rotation_impulse = 0.999f * sp.rotation_impulse + (sp.torque + sp.torque_acceleration) * dt;
        tf.value.rotation = glm::conjugate(glm::quat(dt * sp.rotation_impulse / weighted_moment)) * tf.value.rotation;

        sp.torque              = glm::vec3(0.f);
        sp.torque_acceleration = glm::vec3(0.f);
        // m_did_collide = false;


        constexpr float movement_drag = 0.001f;
        constexpr float rotation_drag = 0.001f;


        /*sp.velocity = (1 - movement_drag) * sp.velocity + sp.acceleration * delta;
        tf.value.position += sp.velocity * delta;


        sp.angular_velocity = slerp((1 - rotation_drag) * sp.angular_velocity, sp.angular_acceleration * sp.angular_velocity, float(delta));
        tf.value.rotation = slerp(tf.value.rotation, sp.angular_velocity * tf.value.rotation, float(delta));*/
    }

private:
    mutable std::mt19937                          gen;
    mutable std::uniform_real_distribution<float> dist;
};

// const float chunk_size = 8.f;
// const float chunk_count = 400;
struct terrain_info
{
    float chunk_size;
    int   chunk_count;
};
class terrain
{
public:
    terrain(const std::filesystem::path& path, float chunk_size, int chunk_count)
          : _heightmap_local(gfx::r8unorm, path)
          , _heightmap(_heightmap_local)
          , _heightmap_view(gfx::imgv_type::image2d, _heightmap)
          , _info{{chunk_size, chunk_count}}
    {
        _sampler.set_wrap(gfx::wrap::u, gfx::wrap_mode::clamp_to_edge);
        _sampler.set_wrap(gfx::wrap::v, gfx::wrap_mode::clamp_to_edge);
    }

    auto terrain_height(glm::vec2 xz)
    {
        glm::vec2 xzi;
        glm::vec2 fract{glm::modf(xz / chunk_size(), xzi)};

        glm::vec2 min = xzi * chunk_size();
        glm::vec2 max = min + chunk_size();

        glm::vec2 v00(min.x, min.y);
        glm::vec2 v10(max.x, min.y);
        glm::vec2 v01(min.x, max.y);
        glm::vec2 v11(max.x, max.y);

        const auto b00 = _heightmap_local.load(glm::vec3(v00 / 1500.f * glm::vec2(_heightmap_local.extents().vec), 0)).r;
        const auto b10 = _heightmap_local.load(glm::vec3(v10 / 1500.f * glm::vec2(_heightmap_local.extents().vec), 0)).r;
        const auto b01 = _heightmap_local.load(glm::vec3(v01 / 1500.f * glm::vec2(_heightmap_local.extents().vec), 0)).r;
        const auto b11 = _heightmap_local.load(glm::vec3(v11 / 1500.f * glm::vec2(_heightmap_local.extents().vec), 0)).r;

        return 80 * glm::mix(glm::mix(b00, b10, fract.x), glm::mix(b01, b11, fract.x), fract.y);
    };

    float                             chunk_size() const noexcept { return _info[0].chunk_size; }
    int                               chunk_count() const noexcept { return _info[0].chunk_count; }
    const gfx::hbuffer<terrain_info>& info() const noexcept { return _info; }
    const gfx::image_view&            map_view() const noexcept { return _heightmap_view; }
    const gfx::sampler&               sampler() const noexcept { return _sampler; }


private:
    gfx::hbuffer<terrain_info> _info;
    gfx::himage                _heightmap_local;
    gfx::image                 _heightmap;
    gfx::image_view            _heightmap_view;
    gfx::sampler               _sampler;
};

class flying_in_circles_system : public gfx::ecs::system
{
public:
    flying_in_circles_system(terrain& t) : _terrain(t)
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

        if (distance(tgt, tf.value.position) < 10.f) {
            const glm::vec2 p(2000.f * dist(gen) - 1000.f, 2000.f * dist(gen) - 1000.f);
            tg.target.position = {p.x, _terrain.terrain_height(p) + 10.f, p.y};
        }
        else
        {
            /* tf.value.rotation =
                 glm::slerp<float>(tf.value.rotation, glm::quatLookAt(dir, glm::vec3(0, 1, 0)), in.prototype->rotation_speed * delta);
             sp.acceleration = in.prototype->acceleration * fwd;
             if (length(sp.velocity) > in.prototype->speed) sp.velocity = normalize(sp.velocity) * in.prototype->speed;
             sp.acceleration -= 200.f * (-(dot(tf.value.left(), dir))) * tf.value.left() * delta * length(sp.velocity);
             sp.acceleration -= 200.f * (-(dot(tf.value.up(), dir))) * tf.value.up() * delta * length(sp.velocity);*/
        }
    }

private:
    terrain&                                      _terrain;
    mutable std::mt19937                          gen;
    mutable std::uniform_real_distribution<float> dist;
};

class plane_movement_system : public gfx::ecs::system
{
public:
    plane_movement_system(terrain& t) : _terrain(t)
    {
        add_component_type(transform_component::id);
        add_component_type(speed_component::id);
        add_component_type(instance_component::id);
        add_component_type(camera_component::id, gfx::ecs::component_flag::optional);
    }

    void update(double delta, gfx::ecs::component_base** components) const override
    {
        auto& tf  = components[0]->as<transform_component>();
        auto& sp  = components[1]->as<speed_component>();
        auto& in  = components[2]->as<instance_component>();
        auto* cam = static_cast<camera_component*>(components[3]);

        const float mass = 10.f;
        if (cam) {
            const auto& context = gfx::context::current();
            const int   thrust =
                (glfwGetKey(context->window(), GLFW_KEY_E) == GLFW_PRESS) - (glfwGetKey(context->window(), GLFW_KEY_Q) == GLFW_PRESS);
            const int v_tilt =
                (glfwGetKey(context->window(), GLFW_KEY_S) == GLFW_PRESS) - (glfwGetKey(context->window(), GLFW_KEY_W) == GLFW_PRESS);
            const int h_tilt =
                (glfwGetKey(context->window(), GLFW_KEY_D) == GLFW_PRESS) - (glfwGetKey(context->window(), GLFW_KEY_A) == GLFW_PRESS);

            sp.impulse = glm::mix(sp.impulse, sp.impulse + 10.f * thrust * tf.value.forward(), float(delta) * in.prototype->acceleration);
            tf.value.rotation =
                glm::slerp(tf.value.rotation, glm::angleAxis<float>(h_tilt, glm::vec3(0, -1, 0)) * tf.value.rotation, float(delta));
            tf.value.rotation =
                glm::slerp(tf.value.rotation, tf.value.rotation * glm::angleAxis<float>(v_tilt, glm::vec3(1, 0, 0)), float(delta));
        }

        if (sp.impulse != glm::vec3(0)) {
            const auto gravity = glm::vec3(0, 9.81f, 0);
            sp.force           = glm::min((cam ? sp.force : glm::vec3(0))
                                    + 0.0001f * glm::length(sp.impulse) / mass / mass
                                          * glm::max(glm::dot(tf.value.forward(), sp.impulse), 0.f) * tf.value.up(),
                                mass * gravity);
        }
    }

private:
    terrain& _terrain;
};

class prototype_renderer
{
public:
    prototype_renderer()
    {
        const std::filesystem::path protos_path = gfx::file("proto/protos.json");
        const auto                  root        = protos_path.parent_path();

        std::ifstream  protos_file(protos_path);
        nlohmann::json protos;
        protos_file >> protos;

        gfx::mesh3d combined;
        for (const auto& json : protos["vehicles"]) {
            const auto name           = json["name"].get<std::string>();
            const auto path           = root / json["mesh"].get<std::string>();
            const auto speed          = json["speed"].get<float>();
            const auto acceleration   = json["acceleration"].get<float>();
            const auto rotation_speed = json["rotspeed"].get<float>();
            const auto logo           = root / json["logo"].get<std::string>();

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
        for (const auto& json : protos["buildings"]) {
            const auto name = json["name"].get<std::string>();
            const auto path = root / json["mesh"].get<std::string>();
            const auto logo = root / json["logo"].get<std::string>();

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

        if (components[2]) return;

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
        auto&       cam = components[0]->as<camera_component>();
        const auto& tf  = components[1]->as<transform_component>();

        int w, h;
        glfwGetFramebufferSize(gfx::context::current()->window(), &w, &h);

        cam.projection.perspective().clip_near     = 0.01f;
        cam.projection.perspective().clip_far      = 400.f;
        cam.projection.perspective().screen_width  = w;
        cam.projection.perspective().screen_height = h;
        _camera.projection_mode                    = cam.projection;
        _camera.transform_mode                     = tf.value;
        _main_component                            = &cam;
    }

    const gfx::camera& camera() const noexcept { return _camera; }
    camera_component*  cam_component() const noexcept { return _main_component; }

private:
    mutable camera_component* _main_component;
    mutable gfx::camera       _camera;
};

class collision_resolver
{
public:
    collision_resolver(terrain& t) : _terrain(t), _colliders(_terrain.chunk_count() * _terrain.chunk_count()) {}

    void clear()
    {
        for (auto& c : _colliders) c.clear();
    }

    void resolve(transform_component& tc, speed_component& sc, sphere_collider_component& scc)
    {
        const auto th = _terrain.terrain_height(glm::vec2(tc.value.position.x, tc.value.position.z));
        if (tc.value.position.y - scc.radius < th) {
            tc.value.position.y          = th + scc.radius;
            const float     eps          = 1.f;
            const auto      get_position = [&](const glm::vec2& v) { return glm::vec3(v.x, _terrain.terrain_height(v), v.y); };
            const glm::vec3 dp0          = get_position(glm::vec2(-eps, -eps));
            const glm::vec3 dp1          = get_position(glm::vec2(-eps, eps));
            const glm::vec3 dp2          = get_position(glm::vec2(eps, eps));
            const glm::vec3 dp3          = get_position(glm::vec2(eps, -eps));
            const glm::vec3 normal_0     = normalize(cross(dp2 - dp0, dp3 - dp1));

            const glm::vec3 ref = glm::reflect(sc.impulse, normal_0);
            sc.impulse          = scc.bounciness * ref;
        }

        const auto chunk_count = _terrain.chunk_count();
        const auto chunk_size  = _terrain.chunk_size();

        const int cx = 15.f * (tc.value.position.x) / 1500.f;
        const int cz = 15.f * (tc.value.position.z) / 1500.f;

        if (cx < 0 || cx >= chunk_count || cz < 0 || cz >= chunk_count) return;

        const int chunk = cx + chunk_count * cz;

        auto& colliders = _colliders[chunk];
        for (const auto c : colliders) {
            auto& t_tc  = *std::get<transform_component*>(c);
            auto& t_sc  = *std::get<speed_component*>(c);
            auto& t_scc = *std::get<sphere_collider_component*>(c);

            if (distance(tc.value.position, t_tc.value.position) < (scc.radius + t_scc.radius)) {
                const glm::vec3 normal = normalize(t_tc.value.position - tc.value.position);
                const glm::vec3 ref    = glm::reflect(sc.impulse, normal);
                const glm::vec3 t_ref  = glm::reflect(t_sc.impulse, -normal);

                sc.impulse   = scc.bounciness * ref;
                t_sc.impulse = t_scc.bounciness * t_ref;

                tc.value.position = t_tc.value.position - (scc.radius + t_scc.radius) * normal;
            }
        }

        for (int i = std::clamp<int>(cx - 1, 0, chunk_count - 1); i <= std::clamp<int>(cx + 1, 0, chunk_count - 1); ++i) {
            for (int j = std::clamp<int>(cz - 1, 0, chunk_count - 1); j <= std::clamp<int>(cz + 1, 0, chunk_count - 1); ++j) {
                const int achunk = i + chunk_count * j;
                colliders.emplace_back(&tc, &sc, &scc);
            }
        }
    }

private:
    terrain&                                                                                                 _terrain;
    std::vector<std::vector<std::tuple<transform_component*, speed_component*, sphere_collider_component*>>> _colliders;
};

class collision_system : public gfx::ecs::system
{
public:
    collision_system(collision_resolver& resolver) : _resolver(resolver)
    {
        add_component_type(transform_component::id);
        add_component_type(speed_component::id);
        add_component_type(sphere_collider_component::id);
    }

    void update(double delta, gfx::ecs::component_base** components) const override
    {
        auto& tc  = components[0]->as<transform_component>();
        auto& sc  = components[1]->as<speed_component>();
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
    camera.projection_mode.perspective().clip_far  = 400.f;

    terrain            main_terrain("heightmap.png", 2.f, 400);
    collision_resolver coll_resolver(main_terrain);

    movement_system       movement;
    plane_movement_system plane_system(main_terrain);
    collision_system      coll_system(coll_resolver);
    gfx::ecs::system_list movement_systems;
    movement_systems.add(movement);
    movement_systems.add(plane_system);
    movement_systems.add(coll_system);

    prototype_renderer    renderer;
    gfx::ecs::system_list graphics_systems;
    prototype_system      proto_system(renderer);
    camera_system         cam_system;
    graphics_systems.add(proto_system);
    graphics_systems.add(cam_system);

    gfx::sampler sampler;
    sampler.set_wrap(gfx::wrap::u, gfx::wrap_mode::repeat);
    sampler.set_wrap(gfx::wrap::v, gfx::wrap_mode::repeat);
    std::mt19937                          gen;
    std::uniform_real_distribution<float> dist;
    std::vector<gfx::ecs::entity>         entities;
    const auto                            spawn_from_prototype = [&](const instance_proto* prototype, const glm::vec3 at) {
        instance_component inst;
        inst.albedo    = glm::vec3(dist(gen), dist(gen), dist(gen));
        inst.prototype = prototype;
        const_cast<instance_proto*>(prototype)->usages++;
        transform_component tf;
        tf.value = gfx::transform(at);
        target_component tgt;
        tgt.target = tf.value;
        speed_component           sp;
        sphere_collider_component collider;
        collider.radius = 0.4f;
        entities.push_back(ecs.create_entity(inst, tf, tgt, sp, collider));
        return entities.back();
    };
    const auto spawn_random_vehicle = [&]() {
        return spawn_from_prototype(&renderer.vehicle_prototypes()[std::clamp(size_t(dist(gen) * renderer.vehicle_prototypes().size()),
                                                                              0ull, renderer.vehicle_prototypes().size())],
                                    glm::vec3(dist(gen), dist(gen), dist(gen)) * 1500.f);
    };

    for (int i = 0; i < 100; ++i) {
        if (i == 0)
            spawn_random_vehicle().add(camera_component{});
        else
            spawn_random_vehicle();
    }

    const auto spawn_random_building = [&]() {
        instance_component inst;
        inst.albedo    = glm::vec3(dist(gen), dist(gen), dist(gen));
        inst.prototype = &renderer.building_prototypes()[std::clamp(size_t(dist(gen) * renderer.building_prototypes().size()), 0ull,
                                                                    renderer.building_prototypes().size())];
        const_cast<instance_proto*>(inst.prototype)->usages++;
        transform_component tf;
        glm::vec2           loc = 1000.f * glm::vec2(dist(gen), dist(gen));
        tf.value                = gfx::transform(glm::vec3(loc.x, 5.f + main_terrain.terrain_height(loc), loc.y));
        entities.push_back(ecs.create_entity(inst, tf));
    };
    for (int i = 0; i < 100; ++i) {
        spawn_random_building();
    }


    gfx::pipe_state::vertex_input mesh_input;
    mesh_input.attributes.emplace_back(0, 0, gfx::rgb32f, offsetof(gfx::vertex3d, position));
    mesh_input.attributes.emplace_back(1, 0, gfx::rg32f, offsetof(gfx::vertex3d, uv));
    mesh_input.attributes.emplace_back(2, 0, gfx::rgb32f, offsetof(gfx::vertex3d, normal));
    mesh_input.bindings.emplace_back(0, sizeof(gfx::vertex3d));
    gfx::binding_layout camera_bindings;
    camera_bindings.push(gfx::binding_type::uniform_buffer);
    gfx::binding_layout mesh_bindings;
    mesh_bindings.push(gfx::binding_type::storage_buffer);
    gfx::binding_layout shadow_bindings;
    shadow_bindings.push(gfx::binding_type::sampled_image).push(gfx::binding_type::uniform_buffer);
    gfx::pipe_state::binding_layouts layouts;
    layouts.layouts.push_back(&camera_bindings);
    layouts.layouts.push_back(&mesh_bindings);
    layouts.layouts.push_back(&shadow_bindings);
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

    const auto                  chunk_count = main_terrain.chunk_count();
    const auto                  chunk_size  = main_terrain.chunk_size();
    gfx::hbuffer<terrain_chunk> chunks_local(chunk_count * chunk_count * 6);
    for (int x = 0; x < chunk_count; ++x) {
        for (int y = 0; y < chunk_count; ++y) {
            chunks_local[6 * x + 6 * chunk_count * y + 0].position =
                glm::vec2((x - (chunk_count / 2.f)) * chunk_size, (y - (chunk_count / 2.f)) * chunk_size) + chunk_size * glm::vec2(0, 0);
            chunks_local[6 * x + 6 * chunk_count * y + 1].position =
                glm::vec2((x - (chunk_count / 2.f)) * chunk_size, (y - (chunk_count / 2.f)) * chunk_size) + chunk_size * glm::vec2(0, 1);
            chunks_local[6 * x + 6 * chunk_count * y + 2].position =
                glm::vec2((x - (chunk_count / 2.f)) * chunk_size, (y - (chunk_count / 2.f)) * chunk_size) + chunk_size * glm::vec2(1, 1);

            chunks_local[6 * x + 6 * chunk_count * y + 3].position =
                glm::vec2((x - (chunk_count / 2.f)) * chunk_size, (y - (chunk_count / 2.f)) * chunk_size) + chunk_size * glm::vec2(0, 0);
            chunks_local[6 * x + 6 * chunk_count * y + 4].position =
                glm::vec2((x - (chunk_count / 2.f)) * chunk_size, (y - (chunk_count / 2.f)) * chunk_size) + chunk_size * glm::vec2(1, 1);
            chunks_local[6 * x + 6 * chunk_count * y + 5].position =
                glm::vec2((x - (chunk_count / 2.f)) * chunk_size, (y - (chunk_count / 2.f)) * chunk_size) + chunk_size * glm::vec2(1, 0);
        }
    }
    gfx::buffer<terrain_chunk> chunks(gfx::buffer_usage::vertex, chunks_local);

    gfx::renderpass_layout shadow_pass;
    shadow_pass.set_depth_stencil_attachment(gfx::d32f);

    gfx::binding_layout terrain_bindings;
    terrain_bindings.push(gfx::binding_type::uniform_buffer).push(gfx::binding_type::sampled_image);
    gfx::binding_layout terrain_style_bindings;
    terrain_style_bindings.push(gfx::binding_type::sampled_image)
        .push(gfx::binding_type::sampled_image)
        .push(gfx::binding_type::uniform_buffer);
    gfx::binding_layout terrain_shadow_bindings;
    terrain_shadow_bindings.push(gfx::binding_type::sampled_image).push(gfx::binding_type::uniform_buffer);
    gfx::pipe_state::binding_layouts terrain_layouts;
    terrain_layouts.layouts.push_back(&terrain_bindings);
    terrain_layouts.layouts.push_back(&terrain_style_bindings);
    gfx::pipe_state::vertex_input terrain_input;
    terrain_input.attributes.emplace_back(0, 0, gfx::rg32f, offsetof(terrain_chunk, position));
    terrain_input.bindings.emplace_back(0, sizeof(terrain_chunk), gfx::input_rate::vertex);
    gfx::pipe_state::input_assembly terrain_assembly;
    terrain_assembly.primitive_topology = gfx::topology::triangle_list;
    gfx::pipe_state::rasterizer terrain_rasterizer;
    terrain_rasterizer.polygon_mode = gfx::poly_mode::fill;
    gfx::pipe_state terrain_state;
    terrain_state.state_rasterizer     = &terrain_rasterizer;
    terrain_state.state_vertex_input   = &terrain_input;
    terrain_state.state_input_assembly = &terrain_assembly;
    terrain_state.state_bindings       = &terrain_layouts;

	terrain_rasterizer.depth_bias_enable = true;
	terrain_rasterizer.depth_bias_constant_factor = -8.f;
	terrain_rasterizer.depth_bias_slope_factor = -8.f;

    const auto terrain_shaders_shadow = {
        gfx::shader(gfx::shader_type::vert, "17_instanced/terrain_shadow.vert"),
        gfx::shader(gfx::shader_type::frag, "17_instanced/mesh_shadow.frag"),
    };
    gfx::graphics_pipeline terrain_pipeline_shadow(terrain_state, shadow_pass, terrain_shaders_shadow);

	terrain_rasterizer.depth_bias_enable = false;

    const auto terrain_shaders = {
        gfx::shader(gfx::shader_type::vert, "17_instanced/terrain.vert"),
        gfx::shader(gfx::shader_type::frag, "17_instanced/terrain.frag"),
    };
    terrain_state.state_multisample = &msaa_state;
    terrain_layouts.layouts.push_back(&terrain_shadow_bindings);
    terrain_rasterizer.cull = gfx::cull_mode::back;
    gfx::graphics_pipeline terrain_pipeline(terrain_state, pass_layout, terrain_shaders);


    gfx::hbuffer<float> time_buffer{2.f};
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
    gfx::graphics_pipeline sky_pipeline(
        sky_state, pass_layout,
        {gfx::shader(gfx::shader_type::vert, "17_instanced/skybox.vert"), gfx::shader(gfx::shader_type::frag, "17_instanced/skybox.frag")});


    const int       shadow_map_count = 5;
    gfx::image      shadow_map(gfx::img_type::attachment, gfx::d32f, gfx::extent(1024, 1024, shadow_map_count), 1);
    gfx::image_view shadow_map_view(gfx::imgv_type::image2d_array, shadow_map);

    std::vector<gfx::framebuffer> shadow_map_framebuffers;
    std::vector<gfx::image_view>  shadow_map_attachments;
    for (int i = 0; i < shadow_map_count; ++i) {
        shadow_map_framebuffers.emplace_back(shadow_map.extents().width, shadow_map.extents().height, 1, shadow_pass);
        auto& view = shadow_map_attachments.emplace_back(gfx::imgv_type::image2d, gfx::d32f, shadow_map, 0, 1, i, 1);
        shadow_map_framebuffers[i].attach(gfx::attachment::depth_stencil, 0, view, gfx::depth_stencil());
    }

    raster.cull                  = gfx::cull_mode::none;
    mesh_state.state_multisample = nullptr;
	raster.depth_bias_enable = true;
	raster.depth_bias_constant_factor = -1.f;
	raster.depth_bias_slope_factor = -1.f;
    const auto shadow_shaders    = {
        gfx::shader(gfx::shader_type::vert, "17_instanced/mesh_shadow.vert"),
        gfx::shader(gfx::shader_type::frag, "17_instanced/mesh_shadow.frag"),
    };
    gfx::graphics_pipeline shadow_mesh_pipeline(mesh_state, shadow_pass, shadow_shaders);

    const float                    vps = 80.f;
    std::vector<gfx::camera>       light_cameras(shadow_map_count);
    std::vector<gfx::camera::data> cam_infos(shadow_map_count);
    for (int i = 0; i < std::size(light_cameras); ++i) {
        light_cameras[i]                         = camera;
        light_cameras[i].transform_mode.position = glm::vec3(100, -150, 100);
        light_cameras[i].transform_mode.rotation =
            glm::quatLookAt(normalize(glm::vec3(0) - light_cameras[i].transform_mode.position), glm::vec3(0, 1, 0));
        const float vpss = vps * (1 << i);
        light_cameras[i].projection_mode =
            gfx::projection(-vpss * 0.5f, vpss * 0.5f, -vpss * 0.5f, vpss * 0.5f, -main_terrain.chunk_size() * main_terrain.chunk_count(),
                            main_terrain.chunk_size() * main_terrain.chunk_count());
        cam_infos[i] = light_cameras[i].info();
    }
    gfx::buffer<gfx::camera::data> light_camera_data(gfx::buffer_usage::uniform, cam_infos);

    std::vector<gfx::binding_set> light_camera_sets;
    std::vector<gfx::binding_set> light_camera_terrain_sets;
    for (int i = 0; i < std::size(light_cameras); ++i) {
        light_camera_sets.emplace_back(camera_bindings).bind(0, 0, light_camera_data, 1, 0 * i);
        auto& s = light_camera_terrain_sets.emplace_back(terrain_bindings);
        s.bind(0, 0, light_camera_data, 1, 0 * i);
        s.bind(1, main_terrain.map_view(), main_terrain.sampler());
    }

    gfx::sampler shadow_sampler;
    shadow_sampler.set_wrap(gfx::wrap::u, gfx::wrap_mode::clamp_to_edge);
    shadow_sampler.set_wrap(gfx::wrap::v, gfx::wrap_mode::clamp_to_edge);
    shadow_sampler.set_compare(true, gfx::compare_op::less_or_equal);
	shadow_sampler.set_anisotropy(false, 0.f);

    gfx::image&     terrain_bump       = res.images_ldr["Rock_023_DISP.png"];
    gfx::image_view terrain_bump_view  = terrain_bump.view(gfx::imgv_type::image2d);
    gfx::image&     terrain_color      = res.images_ldr["grass.jpg"];
    gfx::image_view terrain_color_view = terrain_color.view(gfx::imgv_type::image2d);

    gfx::binding_set terrain_set(terrain_bindings);
    terrain_set.bind(0, *camera_buffer);
    terrain_set.bind(1, main_terrain.map_view(), main_terrain.sampler());
    gfx::binding_set terrain_style_set(terrain_style_bindings);
    terrain_style_set.bind(0, terrain_bump_view, sampler);
    terrain_style_set.bind(1, terrain_color_view, sampler);
    terrain_style_set.bind(2, main_terrain.info());
    gfx::binding_set terrain_shadow_set(terrain_shadow_bindings);
    terrain_shadow_set.bind(0, shadow_map_view, shadow_sampler);
    terrain_shadow_set.bind(1, light_camera_data);

    std::vector<gfx::buffer<instance>> instance_buffers;
    std::vector<gfx::binding_set>      mesh_sets;
    mesh_sets.reserve(context->swapchain()->image_views().size());
    for (int i = 0; i < context->swapchain()->image_views().size(); ++i) {
        mesh_sets.emplace_back(mesh_bindings);
        instance_buffers.emplace_back(gfx::buffer_usage::indirect | gfx::buffer_usage::storage, 1);
        mesh_sets[i].bind(0, instance_buffers.back());
    }
    gfx::binding_set main_camera_set(camera_bindings);
    main_camera_set.bind(0, *camera_buffer);

    int    fc    = 0;
    double t     = 0;
    double ftime = 1.f;
    while (frame()) {
        ++fc;
        t += context->delta();

        if (t > 1) {
            ftime = t / fc;
            t     = 0.0;
            fc    = 0;
        }

        static int  spawn_count             = 1;
        static bool enable_following_camera = false;

        ImGui::Begin("Settings");
        ImGui::DragInt("Spawn Count", &spawn_count, 0.05f, 0, 1000);
        if (ImGui::Button("Spawn Random")) {
            for (int i = 0; i < spawn_count; ++i) spawn_random_vehicle();
        }
        ImGui::Value("Instances", int(renderer.instances().size()));
        ImGui::Value("Framerate", 1.f / float(ftime));
        ImGui::Value("Frametime (ms)", float(ftime * 1000.0));
        ImGui::Checkbox("Follow any", &enable_following_camera);
        if (enable_following_camera && ImGui::Button("Random cam")) {
            gfx::dlog << ecs.remove_components<camera_component>(cam_system.cam_component()->entity);
            entities[int(dist(gen) * entities.size())].add(camera_component{});
        }
        ImGui::Separator();

        for (auto& proto : renderer.vehicle_prototypes()) {
            ImGui::PushID(proto.name.c_str());
            ImGui::Image(&proto.logo_view, ImVec2(64, 64));
            ImGui::SameLine();
            ImGui::Text(proto.name.c_str());
            ImGui::SameLine();
            ImGui::Value("usg: ", proto.usages);
            ImGui::SameLine();
            if (ImGui::Button("Spawn")) {
                spawn_from_prototype(&proto, camera.transform_mode.position + camera.transform_mode.forward() * 8.f);
            }
            ImGui::PopID();
        }

        static bool show_light_cam = false;
        ImGui::Checkbox("Show Light Camera", &show_light_cam);

        ImGui::End();

        renderer.clear();
        coll_resolver.clear();
        ecs.update(context->delta(), movement_systems);
        ecs.update(context->delta(), graphics_systems);
        gfx::buf_copy(instance_buffers[context->swapchain()->current_image()], renderer.instances(), renderer.instances().size());
        mesh_sets[context->swapchain()->current_image()].bind(0, instance_buffers[context->swapchain()->current_image()]);

        camera.transform_mode.position.y =
            main_terrain.terrain_height({camera.transform_mode.position.x, camera.transform_mode.position.z}) + 1.8f;
        current_command->update_buffer(*camera_buffer, 0, camera.info());
        if (enable_following_camera) current_command->update_buffer(*camera_buffer, 0, cam_system.camera().info());
        if (show_light_cam) current_command->update_buffer(*camera_buffer, 0, light_cameras[1].info());

        for (int i = 0; i < std::size(light_cameras); ++i) {
            const float vpss = vps * (1 << i);

            glm::vec3 campos = camera.transform_mode.position;
            campos.y         = main_terrain.terrain_height({campos.x, campos.z});
            light_cameras[i].transform_mode.position =
                (glm::vec3(glm::ivec3(campos * vpss / shadow_map.extents().width)) * shadow_map.extents().width / vpss)
                + 500.f * (light_cameras[i].transform_mode.rotation * glm::vec3(0, 0, 1));

            auto lci       = light_cameras[i].info();
            lci.projection = lci.projection;
            current_command->update_buffer(light_camera_data, 0, lci);
            cam_infos[i] = lci;

            current_command->bind_pipeline(cull_pipeline, {&light_camera_sets[i], &mesh_sets[context->swapchain()->current_image()]});
            current_command->dispatch((renderer.instances().size() + 31) / 32);
            current_command->begin_pass(shadow_map_framebuffers[i]);
            current_command->bind_pipeline(shadow_mesh_pipeline,
                                           {&light_camera_sets[i], &mesh_sets[context->swapchain()->current_image()]});
            current_command->render(renderer);

            current_command->bind_pipeline(terrain_pipeline_shadow, {&light_camera_terrain_sets[i], &terrain_style_set});
            current_command->bind_vertex_buffer(chunks, 0);
            current_command->draw({gfx::u32(chunks.size())});

            current_command->end_pass();
        }
        current_command->update_buffer(light_camera_data, 0, cam_infos.size(), cam_infos.data());

        current_command->bind_pipeline(cull_pipeline, {&main_camera_set, &mesh_sets[context->swapchain()->current_image()]});
        current_command->dispatch((renderer.instances().size() + 31) / 32);
        current_command->begin_pass(*current_framebuffer);
        current_command->bind_pipeline(sky_pipeline, {&sky_set});
        current_command->draw(3);
        current_command->bind_pipeline(mesh_pipeline,
                                       {&main_camera_set, &mesh_sets[context->swapchain()->current_image()], &terrain_shadow_set});
        current_command->render(renderer);
        current_command->bind_pipeline(terrain_pipeline, {&terrain_set, &terrain_style_set, &terrain_shadow_set});
        current_command->bind_vertex_buffer(chunks, 0);
        current_command->draw({gfx::u32(chunks.size())});
        current_command->end_pass();
    }
}