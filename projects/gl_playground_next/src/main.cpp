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

    glDebugMessageCallback(&dbg, nullptr);

    gfx::camera            camera;
    gfx::camera_controller controller(window);

    gfx::scene_file           scene("bunny.dae");
    gl::buffer<gfx::vertex3d> mesh_vertices = scene.meshes[0].vertices;
    gl::buffer<gfx::index32>  mesh_indices  = scene.meshes[0].indices;
    gl::buffer<glm::mat4>     mesh_buffer(glm::scale(glm::vec3(3)), GL_DYNAMIC_STORAGE_BIT);
    gl::vertex_array          mesh_vao;
    mesh_vao.attrib(0).enable(true).format(3, GL_FLOAT, false, offsetof(gfx::vertex3d, position)).bind(0);
    mesh_vao.attrib(1).enable(true).format(2, GL_FLOAT, false, offsetof(gfx::vertex3d, uv)).bind(0);
    mesh_vao.attrib(2).enable(true).format(3, GL_FLOAT, false, offsetof(gfx::vertex3d, normal)).bind(0);
    mesh_vao.vertex_buffer(0, mesh_vertices);
    mesh_vao.element_buffer(mesh_indices);

    struct matrices
    {
        glm::mat4 view, projection;
        glm::vec3 position;
    };
    gl::buffer<matrices> matrix_buffer(matrices(), GL_DYNAMIC_STORAGE_BIT);

    gl::pipeline mesh_pipeline;
    mesh_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("mesh.vert");
    mesh_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("mesh.frag");

    struct light
    {
        gfx::transform       transform;
        gfx::projection      projection{glm::radians(50.f), 1, 1, 0.01f, 1000.f, false, true};
        gl::framebuffer      framebuffer;
        gl::buffer<matrices> matrix_buffer{matrices(), GL_DYNAMIC_STORAGE_BIT};
        gl::sampler sampler;

        struct info
        {
            glm::mat4 shadow_matrix;
            uint64_t  shadow_map;
        };

        info make_info() const
        {
            info i;
            i.shadow_map = sampler.sample(*static_cast<std::shared_ptr<gl::texture>>(framebuffer.at(GL_DEPTH_ATTACHMENT)));
            i.shadow_matrix = glm::mat4(projection) * inverse(transform.matrix());
            return i;
        }
    };

    gl::pipeline mesh_shadow_pipeline;
    mesh_shadow_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("mesh_shadow.vert");
    mesh_shadow_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("mesh_shadow.frag");

    light main_light;
    main_light.transform                        = inverse(glm::lookAt(glm::vec3(0, 14, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1)));
    main_light.framebuffer[GL_DEPTH_ATTACHMENT] = std::make_shared<gl::texture>(GL_TEXTURE_2D, 512, 512, GL_DEPTH32F_STENCIL8, 1);
    main_light.matrix_buffer[0].projection      = main_light.projection;
    main_light.matrix_buffer[0].position        = main_light.transform.position;
    main_light.matrix_buffer[0].view            = inverse(main_light.transform.matrix());
    main_light.matrix_buffer.synchronize();

    gl::buffer<light::info> light_buffer(main_light.make_info(), GL_DYNAMIC_STORAGE_BIT);

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
    while(window->update())
    {
        gui.new_frame();

        ImGui::Begin("Test");
        ImGui::Image(static_cast<uint64_t>(static_cast<mygl::texture>(*static_cast<std::shared_ptr<gl::texture>>(main_light.framebuffer[GL_DEPTH_ATTACHMENT]))), {256, 256});
        ImGui::End();

        main_framebuffer->clear(0, {0.5f, 0.55f, 0.6f, 1.f});
        main_framebuffer->clear(0.f, 0);

        glViewport(0, 0, 512, 512);
        main_light.framebuffer.bind();
        mesh_shadow_pipeline.bind();
        mesh_vao.bind();
        main_light.matrix_buffer.bind(GL_UNIFORM_BUFFER, 0);
        mesh_buffer.bind(GL_UNIFORM_BUFFER, 1);
        glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, nullptr);
        light_buffer[0] = main_light.make_info();
        light_buffer.synchronize();
        
        glViewport(0, 0, 1280, 720);
        controller.update(camera);
        matrix_buffer[0].view       = inverse(camera.transform.matrix());
        matrix_buffer[0].projection = camera.projection;
        matrix_buffer[0].position   = camera.transform.position;
        matrix_buffer.synchronize();
        
        glBindTextureUnit(0, cubemap);
        glBindSampler(0, sampler);

        main_framebuffer->bind();
        mesh_vao.bind();
        matrix_buffer.bind(GL_UNIFORM_BUFFER, 0);
        mesh_buffer.bind(GL_UNIFORM_BUFFER, 1);
        light_buffer.bind(GL_SHADER_STORAGE_BUFFER, 0);
        mesh_pipeline.bind();
        glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, nullptr);

        main_framebuffer->blit(gl::framebuffer::zero(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
        gl::framebuffer::zero().bind();
        gui.render();
    }
}