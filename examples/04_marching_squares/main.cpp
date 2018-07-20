#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>
#include <glm/ext.hpp>

#include <opengl/opengl.hpp>

int main()
{
    gfx::context_options options;
    options.window_title        = "[03] Bidirectional Pathtracer";
    options.window_height       = 720;
    options.window_width        = 1280;
    options.framebuffer_samples = 8;
    auto context                = gfx::context::create(options);
    context->make_current();

    gfx::imgui imgui;

    gfx::device_image texture(gfx::host_image(gfx::rgba8unorm, "Lena.png"));
    gfx::image_view   texture_view(gfx::imgv_type::image_2d, gfx::rgba8unorm, texture, 0, 10, 0, 1);
    gfx::sampler      sampler;

    struct data
    {
        glm::mat4 vp;
        float     offset;
    };
    gfx::host_buffer<data> uniform_buffer(1);
    uniform_buffer[0].offset = 0.5f;

    gfx::camera            camera;
    gfx::camera_controller ctrl;

    gl::pipeline pp;
    pp[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("04_marching_squares/seg.vert");
    pp[GL_GEOMETRY_SHADER] = std::make_shared<gl::shader>("04_marching_squares/seg.geom");
    pp[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("04_marching_squares/seg.frag");

    gfx::vertex_input input;
    input.set_assembly(gfx::topology::point_list);

    context->key_callback.add([&](GLFWwindow*, int k, int s, int a, int m) {
        if(k == GLFW_KEY_R && a == GLFW_PRESS)
            pp.reload();
    });

    while(context->run())
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        imgui.new_frame();

        static float scale = 1.f;
        ImGui::Begin("Controls");
        ImGui::SliderFloat("Offset", &uniform_buffer[0].offset, 0.f, 1.f);
        ImGui::DragFloat("Scale", &scale, 0.1f, 0.f, 1.f);
        if(ImGui::Button("Load Image"))
        {
            if(auto file = gfx::file::open_dialog("Load Image", "./", {"*.png", "*.jpg", "*.bmp"}))
            {
                texture      = gfx::host_image(gfx::rgba8unorm, *file);
                texture_view = gfx::image_view(gfx::imgv_type::image_2d, gfx::rgba8unorm, texture, 0, 10, 0, 1);
            }
        }
        ImGui::End();
        ctrl.update(camera);
        uniform_buffer[0].vp =
                camera.projection_mode.matrix() * glm::inverse(camera.transform_mode.matrix()) * glm::scale(glm::vec3(scale));

        pp.bind();
        glBindTextureUnit(0, handle_cast<mygl::texture>(texture_view));
        glBindSampler(0, handle_cast<mygl::sampler>(sampler));
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, handle_cast<mygl::buffer>(uniform_buffer));
        input.draw((texture.extents().width - 1) * (texture.extents().height - 1));

        imgui.render();
    }

    return 0;
}