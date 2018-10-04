#include <executable.hpp>

void executable::init(gfx::context_options& opt)
{
    opt.window_title        = "[04] Marching Squares";
    opt.window_height       = 720;
    opt.window_width        = 1280;
    opt.framebuffer_samples = gfx::sample_count::x8;
    opt.graphics_api        = gfx::gapi::vulkan;
}

void executable::run()
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

    gfx::pipe_state::input_assembly ass;
    ass.primitive_topology = gfx::topology::point_list;

    gfx::binding_layout layout;
    layout.push(gfx::binding_type::sampled_image).push(gfx::binding_type::uniform_buffer);

    gfx::pipe_state::binding_layouts bindings;
    bindings.layouts.push_back(&layout);

    gfx::pipe_state state;
    state.state_multisample    = &msaa_state;
    state.state_input_assembly = &ass;
    state.state_bindings       = &bindings;

    gfx::graphics_pipeline pipeline(state, pass_layout,
                                    {gfx::shader(gfx::shader_type::vert, "04_marching_squares/seg.vert"),
                                     gfx::shader(gfx::shader_type::geom, "04_marching_squares/seg.geom"),
                                     gfx::shader(gfx::shader_type::frag, "04_marching_squares/seg.frag")});

    context->key_callback.add([&](GLFWwindow*, int k, int s, int a, int m) {
        if (k == GLFW_KEY_R && a == GLFW_PRESS)
        {
            pipeline = gfx::graphics_pipeline(state, pass_layout,
                                              {gfx::shader(gfx::shader_type::vert, "04_marching_squares/seg.vert"),
                                               gfx::shader(gfx::shader_type::geom, "04_marching_squares/seg.geom"),
                                               gfx::shader(gfx::shader_type::frag, "04_marching_squares/seg.frag")});
        }
    });

    gfx::binding_set set(layout);
    set.bind(0, texture_view, sampler);
    set.bind(1, uniform_buffer);

    while (frame())
    {
        static float scale = 1.f;
        ImGui::Begin("Settings");
        ImGui::SliderFloat("Offset", &uniform_buffer[0].offset, 0.f, 1.f);
        ImGui::DragFloat("Scale", &scale, 0.1f, 0.f, 1.f);
        if (ImGui::Button("Load Image"))
        {
            if (auto file = gfx::file::open_dialog("Load Image", "./", {"*.png", "*.jpg", "*.bmp"}))
            {
                texture      = gfx::host_image(gfx::rgba8unorm, *file);
                texture_view = gfx::image_view(gfx::imgv_type::image2d, texture);
                set.bind(0, texture_view, sampler);
            }
        }
        ImGui::End();

        uniform_buffer[0].vp =
			user_entity->get<gfx::camera_component>()->projection.matrix() * glm::inverse(user_entity->get<gfx::camera_component>()->transform.matrix()) * glm::scale(glm::vec3(scale));
	
        current_command->begin_pass(*current_framebuffer);
        current_command->bind_pipeline(pipeline, {&set});
        current_command->draw((texture.extents().width - 1) * (texture.extents().height - 1));
        current_command->end_pass();
    }
}