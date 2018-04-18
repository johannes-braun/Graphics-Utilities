#include <gfx/window.hpp>
#include <gfx/imgui.hpp>
#include <gfx/camera.hpp>
#include <opengl/framebuffer.hpp>
#include <random>

int main()
{
    gl::shader::set_include_directories({ "../shd", SOURCE_DIRECTORY "/global/shd" });
    gfx::window_hints hints;
    hints[GLFW_SAMPLES] = 8;
    const auto window = std::make_shared<gfx::window>(gfx::apis::opengl::name, "[CG3] Voronoi Cones", 1280, 720, hints);
    gfx::imgui imgui(window);

    gl::buffer<gfx::vertex> cone_vertices(GL_DYNAMIC_STORAGE_BIT);
    cone_vertices.emplace_back(glm::vec3(0, 0, 0));
    constexpr int res = 24;
    for (int i=0; i<res+1; ++i)
    {
        const float p = 2 * glm::pi<float>() * i / float(res);
        const float x = glm::sin(p);
        const float z = glm::cos(p);
        cone_vertices.emplace_back(20.f * glm::vec3(x, -0.1, z));
    }

    gl::pipeline pipeline;
    pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("cone.vert");
    pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("cone.frag");

    struct instance
    {
        alignas(16) glm::vec2 offset;
        alignas(8) glm::u8vec4 color;
    };
    gl::buffer<instance> instances(GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);
    instances.map(GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);

    std::mt19937 gen;
    std::uniform_real_distribution<float> dist(0.f, 1.f);
    for (size_t i = 0; i < 5000; i++)
    {
        instances.push_back(instance{ {dist(gen) * 200.f, dist(gen) * 200.f }, glm::u8vec4(dist(gen) * 255, dist(gen) * 255, dist(gen) * 255, 255) });
    }

    gl::buffer<glm::mat4> matrices(1, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);
    matrices.map(GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);

    gfx::camera cam;
    gfx::camera_controller controller(window);

    glEnable(GL_DEPTH_TEST);
    while (window->update())
    {
        imgui.new_frame();
        
        ImGui::Begin("odfuisah");
        ImGui::End();

        controller.update(cam);
        matrices[0] = cam.projection() * cam.view();

        pipeline.bind();
        pipeline.bind_attribute(0, 0, cone_vertices, 3, GL_FLOAT, false);
        pipeline.bind_attribute(1, 1, instances, 2, GL_FLOAT);
        pipeline.bind_attribute(2, 1, instances, 4, GL_UNSIGNED_BYTE, true);
        pipeline.bind_uniform_buffer(0, matrices);
        pipeline.draw_instanced(GL_TRIANGLE_FAN, cone_vertices.size(), instances.size());
        glFinish();

        imgui.render();
    }
}