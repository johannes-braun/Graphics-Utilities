#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>
#include <runnable.hpp>
#include <glm/ext.hpp>

#include <opengl/opengl.hpp>

void runnable::init(gfx::context_options& opt)
{
	opt.window_title        = "[03] Bidirectional Pathtracer";
	opt.window_height       = 720;
	opt.window_width        = 1280;
	opt.framebuffer_samples = 8;
}

void runnable::run()
{
    gfx::device_image texture(gfx::host_image(gfx::rgba8unorm, "Lena.png"));
    gfx::image_view   texture_view(gfx::imgv_type::image2d, texture);
    gfx::sampler      sampler;

    struct data
    {
        glm::mat4 vp;
        float     offset;
    };
    gfx::hbuffer<data> uniform_buffer(1);
    uniform_buffer[0].offset = 0.5f;

	gfx::state_info state;
	state.multisample.sample_shading_enable = true;
	state.multisample.samples = gfx::sample_count(_opt.framebuffer_samples);
	gfx::graphics_pipeline pipeline(gfx::vertex_input{ gfx::topology::point_list }, std::move(state));
	pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "04_marching_squares/seg.vert"));
	pipeline.attach(gfx::shader_type::geom, gfx::shader(gfx::shader_format::text, "04_marching_squares/seg.geom"));
	pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "04_marching_squares/seg.frag"));

    context->key_callback.add([&](GLFWwindow*, int k, int s, int a, int m) {
		if (k == GLFW_KEY_R && a == GLFW_PRESS)
		{
			pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "04_marching_squares/seg.vert"));
			pipeline.attach(gfx::shader_type::geom, gfx::shader(gfx::shader_format::text, "04_marching_squares/seg.geom"));
			pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "04_marching_squares/seg.frag"));
		}
    });

	gfx::descriptor_set set;
	set.set(gfx::descriptor_type::sampled_texture, 0, texture_view, sampler);
	set.set(gfx::descriptor_type::uniform_buffer, 0, uniform_buffer);

	gfx::commands cmd;

    while(frame())
    {
        static float scale = 1.f;
        ImGui::Begin("Controls");
        ImGui::SliderFloat("Offset", &uniform_buffer[0].offset, 0.f, 1.f);
        ImGui::DragFloat("Scale", &scale, 0.1f, 0.f, 1.f);
        if(ImGui::Button("Load Image"))
        {
            if(auto file = gfx::file::open_dialog("Load Image", "./", {"*.png", "*.jpg", "*.bmp"}))
            {
                texture      = gfx::host_image(gfx::rgba8unorm, *file);
                texture_view = gfx::image_view(gfx::imgv_type::image2d, gfx::rgba8unorm, texture, 0, 10, 0, 1);
				set.set(gfx::descriptor_type::sampled_texture, 0, texture_view, sampler);
            }
        }
        ImGui::End();
        uniform_buffer[0].vp =
                camera.projection_mode.matrix() * glm::inverse(camera.transform_mode.matrix()) * glm::scale(glm::vec3(scale));

		cmd.reset();
		cmd.begin_pass(main_framebuffer());
		cmd.bind_pipeline(pipeline);
		cmd.bind_descriptors({ &set, 1 });
		cmd.draw((texture.extents().width - 1) * (texture.extents().height - 1));
		cmd.end_pass();
		cmd.execute();
    }
}