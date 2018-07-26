#include <gfx/gfx.hpp>

#include <runnable.hpp>

void runnable::init(gfx::context_options& options)
{
    options.window_title = "[14] Cubemapper";
    options.debug        = true;
}

void runnable::run()
{
    gfx::commands cmd;

    gfx::hbuffer<float> time_buffer(1);

    gfx::descriptor_set descriptor;
    descriptor.set(gfx::descriptor_type::uniform_buffer, 0, *camera_buffer);
    descriptor.set(gfx::descriptor_type::uniform_buffer, 1, time_buffer);

    const auto cubemap_render_state                        = std::make_shared<gfx::state_info>();
    cubemap_render_state->depth_stencil.depth_write_enable = false;
    gfx::graphics_pipeline pipeline(gfx::vertex_input{}, cubemap_render_state);
    pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "14_cubemapper/skybox.vert"));
    pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "14_cubemapper/skybox.frag"));

    context->key_callback.add([&](GLFWwindow*, int k, int sc, int a, int m) {
        if (k == GLFW_KEY_F5 && a == GLFW_PRESS)
            pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "14_cubemapper/skybox.frag"));
    });

    while (frame()) {
        ImGui::Begin("Bla");
        ImGui::Button("Blub");
        ImGui::End();

        time_buffer[0] = float(glfwGetTime());

        cmd.reset();
        cmd.begin_pass(main_framebuffer());
        cmd.bind_descriptors(&descriptor, 1);
        cmd.set_viewports(&main_viewport, 1, 0);
        cmd.bind_pipeline(pipeline);
        cmd.draw(3);
        cmd.execute();
    }
}
