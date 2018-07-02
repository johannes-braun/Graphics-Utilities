#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>
#include <glm/ext.hpp>

void dbg(GLenum source, GLenum type, unsigned int id, GLenum severity, int length, const char* message, const void* userParam)
{
   gfx::ilog << message;
}

int main()
{
    gfx::context_options options;
    options.window_title        = "[03] Bidirectional Pathtracer";
    options.window_height       = 720;
    options.window_width        = 1280;
    options.framebuffer_samples = 8;
    auto context                = gfx::context::create(options);
    context->make_current();

    glDebugMessageCallback(&dbg, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, false);

    gfx::imgui imgui;

    gfx::host_image image(gfx::rgba8unorm, gfx::image_file("Lena.png", gfx::bits::b8, 4));
    gfx::device_image texture(2, gfx::rgba8unorm, image.extents(), 10);
    texture.level(0) << image;
    texture.generate_mipmaps();
    gfx::image_view texture_view(gfx::view_type::image_2d, gfx::rgba8unorm, texture, 0, 10, 0, 1);
    gfx::sampler    sampler;

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
                image = gfx::host_image(gfx::rgba8unorm, gfx::image_file(file.value(), gfx::bits::b8, 4));
                texture = gfx::device_image(2, gfx::rgba8unorm, image.extents(), 10);
                texture_view = gfx::image_view(gfx::view_type::image_2d, gfx::rgba8unorm, texture, 0, 10, 0, 1);
                texture.level(0) << image;
                texture.generate_mipmaps();
            }
        }
        ImGui::End();
        ctrl.update(camera);
        uniform_buffer[0].vp = camera.projection_mode.matrix() * glm::inverse(camera.transform_mode.matrix()) * glm::scale(glm::vec3(scale));

        pp.bind();
        glBindTextureUnit(0, texture_view);
        glBindSampler(0, sampler);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniform_buffer);
        pp.draw(GL_POINTS, (image.extents().width - 1) * (image.extents().height - 1));

        imgui.render();
    }

    return 0;
}