#include "light.hpp"
#include "mesh.hpp"
#include <gfx/gfx.hpp>
#include <opengl/opengl.hpp>
#include <random>

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
    //log_i << message;
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
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, false);

    gfx::camera            camera;
    gfx::camera_controller controller(window);

    gfx::scene_file scene("sponza.obj");

    std::mt19937 gen;
    std::uniform_real_distribution<float> dist(0.f, 1.f);
    std::vector<std::shared_ptr<gfx::tri_mesh>>           meshes;
    std::vector<std::unique_ptr<gfx::tri_mesh::instance>> instances;
    for(const auto& mesh : scene.meshes)
    {
        auto&& m = meshes.emplace_back(std::make_shared<gfx::tri_mesh>(mesh.vertices, mesh.indices));
        auto&& instance = instances.emplace_back(m->instantiate());
        (instance->transform = mesh.transform).scale = glm::vec3(0.01f);
        instance->material.color = glm::vec3(dist(gen)) * glm::vec3(1.f);
        instance->material.roughness = dist(gen);
    }

    gl::buffer<gfx::camera::data> matrix_buffer(camera.info(), GL_DYNAMIC_STORAGE_BIT);

    gl::pipeline mesh_pipeline;
    mesh_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("mesh.vert");
    mesh_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("mesh.frag");

    gl::pipeline mesh_shadow_pipeline;
    mesh_shadow_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("mesh_shadow.vert");
    mesh_shadow_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("mesh_shadow.frag");

    std::vector<gfx::light> lights(4);
    lights[0].map_camera.transform = inverse(glm::lookAt(glm::vec3(4, 24, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    lights[0].color                = glm::vec4(1.f, 0.6f, 0.4f, 15.f);
    lights[1].map_camera.transform = inverse(glm::lookAt(glm::vec3(4, 20, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    lights[1].color                = glm::vec4(0.3f, 0.7f, 1.f, 15.f);
    lights[2].map_camera.transform = inverse(glm::lookAt(glm::vec3(-7, 8, -4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    lights[2].color                = glm::vec4(0.4f, 0.8f, 0.1f, 13.f);
    lights[3].color                = glm::vec4(0.9f, 0.8f, 0.7f, 10.f);

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

    gl::texture noise_texture(GL_TEXTURE_3D, 128, 128, 128, GL_R16F, 1);
    gl::compute_pipeline simplex_pipeline(std::make_shared<gl::shader>("simplex.comp"));
    int sx = 1.f, sy = 1.f, sz = 1.f;
    gl::buffer<glm::vec3> box_vertices({
                {0, 0, 0},    {0, 0, sz},   {0, sy, sz}, {sx, sy, 0},  {0, 0, 0},   {0, sy, 0},
                {sx, 0, sz},  {0, 0, 0},    {sx, 0, 0},  {sx, sy, 0},  {sx, 0, 0},  {0, 0, 0},
                {0, 0, 0},    {0, sy, sz},  {0, sy, 0},  {sx, 0, sz},  {0, 0, sz},  {0, 0, 0},
                {0, sy, sz},  {0, 0, sz},   {sx, 0, sz}, {sx, sy, sz}, {sx, 0, 0},  {sx, sy, 0},
                {sx, 0, 0},   {sx, sy, sz}, {sx, 0, sz}, {sx, sy, sz}, {sx, sy, 0}, {0, sy, 0},
                {sx, sy, sz}, {0, sy, 0},   {0, sy, sz}, {sx, sy, sz}, {0, sy, sz}, {sx, 0, sz}});
    gl::vertex_array box_vao;
    box_vao.attrib(0).enable(true).format(3, GL_FLOAT, false, 0).bind(0);
    box_vao.vertex_buffer(0, box_vertices);
    gl::pipeline box_pipeline;
    box_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("inbox.vert");
    box_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("noise_texture.frag");
    gl::sampler noise_sampler;
    gl::buffer<float> time(static_cast<float>(glfwGetTime()), GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);
    time.map(GL_MAP_WRITE_BIT);
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    gl::query simplex_query(GL_TIME_ELAPSED);
    gl::query light_query(GL_TIME_ELAPSED);
    gl::query update_query(GL_TIME_ELAPSED);
    gl::query render_query(GL_TIME_ELAPSED);
    while(window->update())
    {
        gui.new_frame();

        gl::framebuffer::zero().clear(0, {0.3f, 0.3f, 0.4f, 1.f});
        gl::framebuffer::zero().clear(0.f, 0);

        light_query.start();
        mesh_shadow_pipeline.bind();
        for(auto& l : lights)
        {
            if(l.begin_shadowmap())
            {
                for(const auto& i : instances)
                    i->render(l.map_camera);
            }
        }
        light_query.finish();

        update_query.start();

        light_buffer.clear();
        for(const auto& l : lights)
            if(l.enabled)
                light_buffer.push_back(l.make_info());

        lights[1].map_camera.transform = inverse(
                glm::lookAt(glm::vec3(7 * glm::sin(static_cast<float>(glfwGetTime())), 20, 7 * glm::cos(static_cast<float>(glfwGetTime()))),
                            glm::vec3(0, 0, 0),
                            glm::vec3(0, 1, 0)));

        lights[3].map_camera.transform.position = glm::lerp(lights[3].map_camera.transform.position, camera.transform.position, static_cast<float>(10.f * window->delta_time()));
        lights[3].map_camera.transform.rotation = glm::slerp(lights[3].map_camera.transform.rotation, camera.transform.rotation, static_cast<float>(10.f * window->delta_time()));

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
        
        gl::framebuffer::zero().bind();
        matrix_buffer.bind(GL_UNIFORM_BUFFER, 0);
        light_buffer.bind(GL_SHADER_STORAGE_BUFFER, 0);
        mesh_pipeline.bind();
        for(const auto& i : instances)
            i->render(camera);

        render_query.finish();
        
        gl::framebuffer::zero().bind();
        simplex_query.start();
        glBindImageTexture(0, noise_texture, 0, true, 0, GL_WRITE_ONLY, GL_R16F);
        simplex_pipeline.dispatch(128, 128, 128);

        time[0] = static_cast<float>(glfwGetTime());
        glEnable(GL_BLEND);
        glCullFace(GL_FRONT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        matrix_buffer.bind(GL_UNIFORM_BUFFER, 0);
        time.bind(GL_UNIFORM_BUFFER, 5);
        glBindSampler(0, noise_sampler);
        glBindTextureUnit(0, noise_texture);
        box_pipeline.bind();
        box_vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, box_vertices.size());
        glCullFace(GL_BACK);
        glDisable(GL_BLEND);

        simplex_query.finish();

        ImGui::Begin("Test");
        ImGui::Value("Simplex", static_cast<float>(simplex_query.get<uint64_t>() / 1'000'000.0));
        ImGui::Value("Shadow", static_cast<float>(light_query.get<uint64_t>() / 1'000'000.0));
        ImGui::Value("Update", static_cast<float>(update_query.get<uint64_t>() / 1'000'000.0));
        ImGui::Value("Render", static_cast<float>(render_query.get<uint64_t>() / 1'000'000.0));

        ImGui::Spacing();
        for(int i=0; i<lights.size(); ++i)
        {
            const std::string id = "light[" + std::to_string(i) + "]";
            ImGui::DragFloat4(id.c_str(), glm::value_ptr(lights[i].color), 0.001f, 0.f, 1000.f);
            ImGui::Checkbox((id+".use_shadowmap").c_str(), &lights[i].use_shadowmap);
            ImGui::Checkbox((id+".enabled").c_str(), &lights[i].enabled);

            if(i != 0)
                ImGui::Spacing();
        }
        ImGui::End();
        gui.render();
    }
}