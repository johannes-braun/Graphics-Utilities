#include <gfx/gfx.hpp>
#include <glm/ext.hpp>

int main()
{
    gfx::context_options options;
    options.window_title        = "[03] Bidirectional Pathtracer";
    options.window_height       = 720;
    options.window_width        = 1280;
    options.framebuffer_samples = 8;
    auto context                = gfx::context::create(options);
    context->make_current();

    gfx::imgui      imgui;
    gfx::image_file img("Lena.png", gfx::bits::b8, 4);
    gl::texture     texture(GL_TEXTURE_2D, img.width, img.height, GL_RGBA8, 1);
    texture.assign(GL_RGBA, GL_UNSIGNED_BYTE, img.bytes());
    gl::sampler sampler;

    struct data
    {
        glm::mat4 vp;
        float     offset;
    };
    gfx::host_buffer<data>   buf(1);
    gfx::device_buffer<data> uniform_buffer(gfx::buffer_usage::uniform, 1);
    buf[0].offset = 0.5f;

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
        ImGui::SliderFloat("Offset", &buf[0].offset, 0.f, 1.f);
        ImGui::DragFloat("Scale", &scale, 0.1f, 0.f, 1.f);
        if(ImGui::Button("Load Image"))
        {
            if(auto file = gfx::file::open_dialog("Load Image", "./", {"*.png", "*.jpg", "*.bmp"}))
            {
                img     = gfx::image_file(file.value(), gfx::bits::b8, 4);
                texture = gl::texture(GL_TEXTURE_2D, img.width, img.height, GL_RGBA8, 1);
                texture.assign(GL_RGBA, GL_UNSIGNED_BYTE, img.bytes());
            }
        }
        ImGui::End();
        ctrl.update(camera);
        buf[0].vp = camera.projection.matrix() * glm::inverse(camera.transform.matrix()) *
                    glm::scale(glm::vec3(scale));

        uniform_buffer << buf;

        pp.bind();
        sampler.bind(0);
        texture.bind(0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniform_buffer.api_handle<mygl::buffer>());
        pp.draw(GL_POINTS, (img.width - 1) * (img.height - 1));

        imgui.render();
    }

    return 0;
}