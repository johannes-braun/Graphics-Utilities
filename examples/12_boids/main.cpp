#include <executable.hpp>
#include <random>

void executable::init(gfx::context_options& opt)
{
    opt.framebuffer_samples = gfx::sample_count::x8;
    opt.window_title        = "[12] Boids";
    opt.window_height       = 900;
    opt.window_width        = 900;
	opt.graphics_api = gfx::gapi::vulkan;
}

struct boid
{
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec4 color;
};

struct mouse
{
    glm::vec2 position;
	float dt;
};

void executable::run()
{
    std::mt19937                          gen;
    std::uniform_real_distribution<float> dist(-1.f, 1.f);

    gfx::hbuffer<boid> boids(800);
    const auto         genboid = [&]() {
        boid b;
        b.position = {dist(gen), dist(gen)};
        b.velocity = {0, 0};
        b.color    = {dist(gen), dist(gen), dist(gen), dist(gen)};
        b.color    = (b.color + 1.f) * 0.5f;
        b.color    = glm::vec4((glm::u8vec4(b.color * 255.f) / uint8_t(200)) * uint8_t(200)) / 255.f;
        return b;
    };
    std::generate(boids.begin(), boids.end(), genboid);

    std::vector<gfx::buffer<boid>> ping_pong_boids;
    ping_pong_boids.emplace_back(gfx::buffer_usage::storage | gfx::buffer_usage::vertex, boids);
    ping_pong_boids.emplace_back(gfx::buffer_usage::storage | gfx::buffer_usage::vertex, boids);

    gfx::binding_layout base_binding_layout;
	base_binding_layout.push(gfx::binding_type::storage_buffer).push(gfx::binding_type::storage_buffer).push(gfx::binding_type::uniform_buffer);
	gfx::binding_layout draw_layout;
	draw_layout.push(gfx::binding_type::sampled_image);

    gfx::pipe_state::binding_layouts layout;
	layout.layouts.push_back(&base_binding_layout);
    gfx::compute_pipeline boid_update(layout, gfx::shader(gfx::shader_type::comp, "12_boids/update.comp"));

	gfx::pipe_state::vertex_input input;
	input.attributes.emplace_back(0, 0, gfx::rg32f, offsetof(boid, position));
	input.attributes.emplace_back(1, 0, gfx::rg32f, offsetof(boid, velocity));
	input.attributes.emplace_back(2, 0, gfx::rgba32f, offsetof(boid, color));
	input.bindings.emplace_back(0, sizeof(boid));

	gfx::pipe_state::input_assembly ass;
	ass.primitive_topology = gfx::topology::point_list;

	gfx::pipe_state::depth_stencil dep;
	dep.depth_bounds_test_enable = false;

	layout.layouts[0] = &draw_layout;

	gfx::pipe_state state;
	state.state_bindings = &layout;
	state.state_multisample = &msaa_state;
	state.state_vertex_input = &input;
	state.state_input_assembly = &ass;
	state.state_depth_stencil = &dep;

	gfx::graphics_pipeline boid_render(state, pass_layout, {
		gfx::shader(gfx::shader_type::vert, "12_boids/render_points.vert"),
		gfx::shader(gfx::shader_type::geom, "12_boids/render_points.geom"),
		gfx::shader(gfx::shader_type::frag, "12_boids/render_points.frag"),
	});

    gfx::hbuffer<mouse> mouse_pos(1);

    gfx::image img(gfx::himage(gfx::rgba8unorm, "bee.png"));
    img.generate_mipmaps();
    gfx::image_view imgv = img.view(gfx::imgv_type::image2d);
    gfx::sampler    sampler;

	gfx::binding_set a(base_binding_layout);
	gfx::binding_set b(base_binding_layout);
	gfx::binding_set* sets[2]{ &a, &b };
    sets[0]->bind(0, ping_pong_boids[0]);
    sets[0]->bind(1, ping_pong_boids[1]);
    sets[0]->bind(2, mouse_pos);
    sets[1]->bind(0, ping_pong_boids[1]);
    sets[1]->bind(1, ping_pong_boids[0]);
    sets[1]->bind(2, mouse_pos);
    int current_set = 0;

    gfx::binding_set draw_set(draw_layout);
    draw_set.bind(0, imgv, sampler);

    int f = 0;
    while (frame()) {
      /*  if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                bool u = false;
                ImGui::MenuItem("Open", "Ctrl+O", &u);
                ImGui::MenuItem("Save", "Ctrl+S", &u);
                ImGui::Separator();
                ImGui::MenuItem("Close", "", &u);
                ImGui::MenuItem("Exit", "", &u);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }*/

        ImGui::Begin("Settings");
        ImGui::Value("Current set", current_set);

        ++f;
        if (false && f % 10000 == 0) {
            gfx::buf_copy(boids, ping_pong_boids[(current_set + 1) % 2], boids.size());
            boids.push_back(genboid());
            gfx::buf_copy(ping_pong_boids[0], boids, boids.size());
            gfx::buf_copy(ping_pong_boids[1], boids, boids.size());
			sets[0]->bind(0, ping_pong_boids[0]);
			sets[0]->bind(1, ping_pong_boids[1]);
			sets[1]->bind(0, ping_pong_boids[1]);
			sets[1]->bind(1, ping_pong_boids[0]);
        }
        ImGui::End();

        double x = 0, y = 0;
        int    w = 0, h = 0;
        glfwGetCursorPos(context->window(), &x, &y);
        glfwGetFramebufferSize(context->window(), &w, &h);
        mouse_pos[0].position = (glm::dvec2(x, y) / glm::dvec2(w, h)) * 2.0 - 1.0;
		mouse_pos[0].dt = context->delta();

		current_command->bind_pipeline(boid_update, { sets[current_set] });
		current_command->dispatch((boids.size() + 127)/128);

		current_command->begin_pass(*current_framebuffer);
		current_command->bind_pipeline(boid_render, { &draw_set });
		current_command->bind_vertex_buffer(ping_pong_boids[current_set], 0);
		current_command->draw(boids.size());
		current_command->end_pass();

        current_set = (current_set + 1) % 2;
    }
}