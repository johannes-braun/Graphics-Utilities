#include <executable.hpp>

void executable::init(gfx::context_options& options)
{
    options.window_title        = "[14] Cubemapper";
    options.debug               = true;
    options.framebuffer_samples = gfx::sample_count::x8;
    options.graphics_api        = gfx::gapi::vulkan;
}

void executable::run()
{
    gfx::hbuffer<float> time_buffer(1);
    gfx::binding_layout bindings;
    bindings.push(gfx::binding_type::uniform_buffer).push(gfx::binding_type::uniform_buffer);
    gfx::pipe_state::binding_layouts binding_state;
    binding_state.layouts.push_back(&bindings);
    gfx::binding_set set(bindings);
    set.bind(0, *camera_buffer);
    set.bind(1, time_buffer);
    gfx::pipe_state::depth_stencil depth;
    depth.depth_write_enable = false;
    depth.depth_test_enable  = true;
    gfx::pipe_state state;
    state.state_bindings      = &binding_state;
    state.state_depth_stencil = &depth;
    state.state_multisample   = &msaa_state;
    gfx::graphics_pipeline pipeline(state, pass_layout,
                                    {gfx::shader(gfx::shader_type::vert, "14_cubemapper/skybox.vert"),
                                     gfx::shader(gfx::shader_type::frag, "14_cubemapper/skybox.frag")});

    context->key_callback.add([&](GLFWwindow*, int k, int sc, int a, int m) {
        if (k == GLFW_KEY_F5 && a == GLFW_PRESS)
            pipeline = gfx::graphics_pipeline(state, pass_layout,
                                              {gfx::shader(gfx::shader_type::vert, "14_cubemapper/skybox.vert"),
                                               gfx::shader(gfx::shader_type::frag, "14_cubemapper/skybox.frag")});
    });

    while (frame()) {
        ImGui::Begin("Settings");
        ImGui::Button("Blub");
        ImGui::End();

        time_buffer[0] = float(glfwGetTime());

		current_command->begin_pass(*current_framebuffer);
		current_command->bind_pipeline(pipeline, { &set });
		current_command->draw(3);
		current_command->end_pass();
    }
}
