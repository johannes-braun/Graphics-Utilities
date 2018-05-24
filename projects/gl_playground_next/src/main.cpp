#include "light.hpp"
#include "mesh.hpp"
#include <gfx/gfx.hpp>
#include <opengl/opengl.hpp>

std::shared_ptr<gfx::window>     window;
std::unique_ptr<gl::framebuffer> main_framebuffer;

void build_framebuffer(int width, int height)
{
    main_framebuffer = std::make_unique<gl::framebuffer>();
    main_framebuffer->at(GL_COLOR_ATTACHMENT0) =
            std::make_shared<gl::texture>(GL_TEXTURE_2D_MULTISAMPLE, width, height, gl::samples::x8, GL_RGBA16F);
    main_framebuffer->at(GL_DEPTH_ATTACHMENT) =
            std::make_shared<gl::texture>(GL_TEXTURE_2D_MULTISAMPLE, width, height, gl::samples::x8, GL_DEPTH32F_STENCIL8, 1);
}
void dbg(GLenum source, GLenum type, unsigned int id, GLenum severity, int length, const char* message, const void* userParam)
{
    log_i << message;
}

int main()
{
    gl::shader::set_include_directories(std::vector<gfx::files::path>{"../shd", SOURCE_DIRECTORY "/global/shd"});
    gfx::window_hints hints{{GLFW_SAMPLES, 8}, {GLFW_OPENGL_DEBUG_CONTEXT, true}};
    window = std::make_shared<gfx::window>(gfx::apis::opengl::name, "[GL] PlaygroundNext", 1280, 720, hints);
    gfx::imgui gui(window);
    window->key_callback.add([](GLFWwindow*, int k, int sc, int ac, int md) {
        if(k == GLFW_KEY_F5 && ac == GLFW_PRESS)
            gl::pipeline::reload_all();
    });
    glfwSwapInterval(0);
    window->set_max_framerate(5000.0);

    glDebugMessageCallback(&dbg, nullptr);

    gfx::camera            camera;
    gfx::camera_controller controller(window);

    gfx::scene_file scene("shadowtest2.dae");

    std::vector<std::shared_ptr<gfx::tri_mesh>>           meshes;
    std::vector<std::unique_ptr<gfx::tri_mesh::instance>> instances;
    for(const auto& mesh : scene.meshes)
    {
        auto&& m = meshes.emplace_back(std::make_shared<gfx::tri_mesh>(mesh.vertices, mesh.indices));
        instances.emplace_back(m->instantiate())->transform = mesh.transform;
    }

    gl::buffer<gfx::camera::data> matrix_buffer(camera.info(), GL_DYNAMIC_STORAGE_BIT);

    gl::pipeline mesh_pipeline;
    mesh_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("mesh.vert");
    mesh_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("mesh.frag");

    gl::pipeline mesh_shadow_pipeline;
    mesh_shadow_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("mesh_shadow.vert");
    mesh_shadow_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("mesh_shadow.frag");

    std::vector<gfx::light> lights(3);
    lights[0].map_camera.transform = inverse(glm::lookAt(glm::vec3(4, 14, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    lights[0].color                = glm::vec4(1.f, 0.6f, 0.4f, 15.f);
    lights[1].map_camera.transform = inverse(glm::lookAt(glm::vec3(4, 10, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    lights[1].color                = glm::vec4(0.3f, 0.7f, 1.f, 15.f);
    lights[2].map_camera.transform = inverse(glm::lookAt(glm::vec3(-7, 8, -4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    lights[2].color                = glm::vec4(0.4f, 0.8f, 0.1f, 13.f);

    gl::buffer<gfx::light::info> light_buffer(GL_DYNAMIC_STORAGE_BIT);
    for(const auto& l : lights)
        light_buffer.push_back(l.make_info());

    const auto[w, h, c] = gfx::image_file::info("indoor/posx.hdr");
    gl::texture cubemap(GL_TEXTURE_CUBE_MAP, w, h, GL_R11F_G11F_B10F);
    cubemap.assign(0, 0, 0, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/posx.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0, 0, 1, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/negx.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0, 0, 2, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/posy.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0, 0, 3, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/negy.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0, 0, 4, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/posz.hdr", gfx::bits::b32, 3).bytes());
    cubemap.assign(0, 0, 5, w, h, 1, GL_RGB, GL_FLOAT, gfx::image_file("indoor/negz.hdr", gfx::bits::b32, 3).bytes());
    cubemap.generate_mipmaps();
    const gl::sampler sampler;

    build_framebuffer(1280, 720);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    gl::query light_query(GL_TIME_ELAPSED);
    gl::query update_query(GL_TIME_ELAPSED);
    gl::query render_query(GL_TIME_ELAPSED);
    while(window->update())
    {
        gui.new_frame();

        main_framebuffer->clear(0, {0.5f, 0.55f, 0.6f, 1.f});
        main_framebuffer->clear(0.f, 0);

        light_query.start();
        for(auto& l : lights)
        {
            if(l.begin_shadowmap())
            {
                mesh_shadow_pipeline.bind();
                for(const auto& i : instances)
                    i->render();
            }
        }
        light_query.finish();

        update_query.start();

        light_buffer.clear();
        for(const auto& l : lights)
            light_buffer.push_back(l.make_info());
        light_buffer.synchronize();

        lights[1].map_camera.transform = inverse(
                glm::lookAt(glm::vec3(7 * glm::sin(static_cast<float>(glfwGetTime())), 10, 7 * glm::cos(static_cast<float>(glfwGetTime()))),
                            glm::vec3(0, 0, 0),
                            glm::vec3(0, 1, 0)));

        glViewport(0, 0, 1280, 720);
        controller.update(camera);
        matrix_buffer[0].view       = inverse(camera.transform.matrix());
        matrix_buffer[0].projection = camera.projection;
        matrix_buffer[0].position   = camera.transform.position;
        matrix_buffer.synchronize();
        update_query.finish();

        render_query.start();
        glBindTextureUnit(0, cubemap);
        glBindSampler(0, sampler);

        main_framebuffer->bind();
        matrix_buffer.bind(GL_UNIFORM_BUFFER, 0);
        light_buffer.bind(GL_SHADER_STORAGE_BUFFER, 0);
        mesh_pipeline.bind();
        for(const auto& i : instances)
            i->render();

        main_framebuffer->blit(gl::framebuffer::zero(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
        gl::framebuffer::zero().bind();

        render_query.finish();
        ImGui::Begin("Test");
        ImGui::Value("Shadow", static_cast<float>(light_query.get<uint64_t>() / 1'000'000.0));
        ImGui::Value("Update", static_cast<float>(update_query.get<uint64_t>() / 1'000'000.0));
        ImGui::Value("Render", static_cast<float>(render_query.get<uint64_t>() / 1'000'000.0));

        ImGui::Spacing();
        ImGui::DragFloat4("lights[0]", glm::value_ptr(lights[0].color), 0.001f, 0.f, 1000.f);
        ImGui::Checkbox("lights[0].use_shadowmap", &lights[0].use_shadowmap);
        ImGui::DragFloat4("lights[1]", glm::value_ptr(lights[1].color), 0.001f, 0.f, 1000.f);
        ImGui::Checkbox("lights[1].use_shadowmap", &lights[1].use_shadowmap);
        ImGui::DragFloat4("lights[2]", glm::value_ptr(lights[2].color), 0.001f, 0.f, 1000.f);
        ImGui::Checkbox("lights[2].use_shadowmap", &lights[2].use_shadowmap);
        ImGui::End();
        gui.render();
    }
}