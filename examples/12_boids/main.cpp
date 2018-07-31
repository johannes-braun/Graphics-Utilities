#include <gfx/gfx.hpp>
#include <runnable.hpp>
#include <random>

void runnable::init(gfx::context_options& opt)
{
	opt.framebuffer_samples = 8;
	opt.window_title = "[12] Boids";
	opt.window_height = 900;
	opt.window_width = 900;
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
};

void runnable::run()
{
	std::mt19937 gen;
	std::uniform_real_distribution<float> dist(-1.f, 1.f);

	gfx::hbuffer<boid> boids(500);
	std::generate(boids.begin(), boids.end(), [&]() {
		boid b;
		b.position ={ dist(gen), dist(gen) };
		b.velocity ={ 0, 0 };
		b.color ={ dist(gen), dist(gen), dist(gen), dist(gen) };
		b.color = (b.color + 1.f) * 0.5f;
		return b;
	});

	std::vector<gfx::buffer<boid>> ping_pong_boids;
	ping_pong_boids.emplace_back(gfx::buffer_usage::storage, boids);
	ping_pong_boids.emplace_back(gfx::buffer_usage::storage, boids);

	gfx::compute_pipeline boid_update(gfx::shader(gfx::shader_format::text, "12_boids/update.comp"));

	auto input = std::make_shared<gfx::vertex_input>();
	input->set_assembly(gfx::topology::point_list);
	input->add_attribute(0, gfx::rg32f, offsetof(boid, position));
	input->add_attribute(0, gfx::rg32f, offsetof(boid, velocity));
	input->add_attribute(0, gfx::rgba32f, offsetof(boid, color));
	
	auto state = std::make_shared<gfx::state_info>();
	state->depth_stencil.depth_test_enable = false;
	state->multisample.sample_shading_enable = true;
	state->multisample.samples = gfx::sample_count::x8;

	gfx::graphics_pipeline boid_render(input, state);
	boid_render.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "12_boids/render_points.vert"));
	boid_render.attach(gfx::shader_type::geom, gfx::shader(gfx::shader_format::text, "12_boids/render_points.geom"));
	boid_render.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "12_boids/render_points.frag"));

	gfx::hbuffer<mouse> mouse_pos(1);

	gfx::image img(gfx::himage(gfx::rgba8unorm, "bee.png"));
	img.generate_mipmaps();
	gfx::image_view imgv = img.view(gfx::imgv_type::image2d);
	gfx::sampler sampler;

	gfx::descriptor_set sets[2];
	sets[0].set(gfx::descriptor_type::storage_buffer, 0, ping_pong_boids[0]);
	sets[0].set(gfx::descriptor_type::storage_buffer, 1, ping_pong_boids[1]);
	sets[0].set(gfx::descriptor_type::uniform_buffer, 0, mouse_pos);
	sets[1].set(gfx::descriptor_type::storage_buffer, 0, ping_pong_boids[1]);
	sets[1].set(gfx::descriptor_type::storage_buffer, 1, ping_pong_boids[0]);
	sets[1].set(gfx::descriptor_type::uniform_buffer, 0, mouse_pos);
	int current_set = 0;

	gfx::descriptor_set draw_set;
	draw_set.set(gfx::descriptor_type::sampled_texture, 0, imgv, sampler);

	gfx::commands cmd;

	while (frame())
	{
		ImGui::Begin("Settings");
		ImGui::Value("Current set", current_set);
		ImGui::End();

		double x =0, y =0;
		int w =0, h =0;
		glfwGetCursorPos(context->window(), &x, &y);
		glfwGetFramebufferSize(context->window(), &w, &h);
		mouse_pos[0].position = (glm::dvec2(x, h-y) / glm::dvec2(w, h)) * 2.0 - 1.0;

		cmd.reset();
		cmd.bind_pipeline(boid_update);
		cmd.bind_descriptors({ &sets[current_set], 1 });
		cmd.dispatch_compute((boids.size() + 127) / 128);

		cmd.begin_pass(main_framebuffer());
		cmd.bind_pipeline(boid_render);
		cmd.bind_descriptors({ &draw_set, 1 });
		cmd.bind_vertex_buffer(0, ping_pong_boids[current_set]);
		cmd.draw(boids.size());
		cmd.end_pass();
		cmd.execute();

		current_set = (current_set+1)%2;
	}
}