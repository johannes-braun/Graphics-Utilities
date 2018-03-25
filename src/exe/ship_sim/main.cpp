#include <io/window.hpp>
#include <io/camera.hpp>

#include <res/geometry.hpp>

#include <opengl/pipeline.hpp>
#include <opengl/framebuffer.hpp>

struct spring
{
    int particle1;
    int particle2;
    float stiff;
    float damping;
    float length;
    float _p;
};

int main()
{
    glfwWindowHint(GLFW_SAMPLES, 8);
    io::window window(io::api::opengl, 1280, 720, "Ship Sim");
    io::camera camera;
    io::default_cam_controller controller;

    res::geometry_file ship_file = res::load_geometry("../res/ship.dae");
    gl::buffer<res::vertex> ship_vbo(ship_file.meshes.get_by_index(0).vertices.begin(), ship_file.meshes.get_by_index(0).vertices.end());
    gl::buffer<res::index32> ship_ibo(ship_file.meshes.get_by_index(0).indices.begin(), ship_file.meshes.get_by_index(0).indices.end());

    gl::pipeline ship_pipeline;
    ship_pipeline[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("ship_sim/ship.vert");
    ship_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("ship_sim/ship.frag");

    struct data { glm::mat4 view_mat, projection_mat; };
    struct model { glm::mat4 model_mat, normal_mat; };
    gl::buffer<data> data_buffer(1, GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<model> model_buffer(1, GL_DYNAMIC_STORAGE_BIT);

    constexpr int size = 8;
    gl::buffer<res::vertex> sail_vertices(size * size, GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<res::index32> sail_indices((size - 1) * (size - 1) * 6, GL_DYNAMIC_STORAGE_BIT);
    int i=0; for (int y = 0; y < size; ++y) for (int x = 0; x < size; ++x)
    {
        sail_vertices[y * size + x].position = { x, y, 0 };
        sail_vertices[y * size + x].normal = { 0, 0, 1 };
        sail_vertices[y * size + x].uv = { x / float(size), y / float(size) };
        sail_vertices[y * size + x].meta = 0;
        sail_vertices[y * size + x].meta2 = 0;
        sail_vertices[y * size + x].meta3 = 0;

        if (x < size - 1 && y < size - 1)
        {
            sail_indices[i++] = y * size + x;
            sail_indices[i++] = (y + 1)*size + x;
            sail_indices[i++] = y * size + x + 1;

            sail_indices[i++] = (y + 1)*size + x;
            sail_indices[i++] = y * size + x + 1;
            sail_indices[i++] = (y + 1)*size + x + 1;
        }
    }
    sail_vertices.synchronize();
    sail_indices.synchronize();

    gl::buffer<spring> springs(GL_DYNAMIC_STORAGE_BIT);
    for (int s = 0; s < size*size; ++s) for (int e = s + 1; e < size * size; ++e)
    {
        const glm::vec3 p1 = sail_vertices[s].position;
        const glm::vec3 p2 = sail_vertices[e].position;

        if (distance(p1, p2) < 2.01f)
            springs.push_back(spring{ s, e, 40.f, 0.001f, distance(p1, p2) });
    }

    struct sail_sim_data { 
        uintptr_t spring_results;
        uintptr_t particle_velocities;
        uintptr_t springs;
        uintptr_t vertices;
        uintptr_t vertex_springs; 
        int spring_count;
        int vertex_count;
    };

    struct timings
    {
        float delta;
        float deltav;
    };

    gl::compute_pipeline sail_spring_pipeline(std::make_shared<gl::shader>("ship_sim/sails/spring.comp"));
    gl::compute_pipeline sail_integrate_pipeline(std::make_shared<gl::shader>("ship_sim/sails/integrate.comp"));

    gl::buffer<sail_sim_data> sail_sim_data_buffer(1, GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<timings> sail_timings_buffer(1, GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<glm::vec4> spring_results((12+1) * sail_vertices.size(), GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<uint32_t> vertex_springs(sail_vertices.size(), GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<glm::vec4> sail_velocities(sail_vertices.size(), GL_DYNAMIC_STORAGE_BIT);
    sail_sim_data_buffer[0].particle_velocities = sail_velocities.handle();
    sail_sim_data_buffer[0].spring_results = spring_results.handle();
    sail_sim_data_buffer[0].springs = springs.handle();
    sail_sim_data_buffer[0].vertices = sail_vertices.handle();
    sail_sim_data_buffer[0].vertex_springs = vertex_springs.handle();
    sail_sim_data_buffer[0].spring_count = springs.size();
    sail_sim_data_buffer[0].vertex_count = sail_vertices.size();
    sail_sim_data_buffer.synchronize();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glEnable(GL_DEPTH_TEST);
    int frame = 0;
    while (window.update())
    {
        gl::framebuffer::zero().clear(0, { 0.5f, 0.9f, 1.f, 1.f });
        gl::framebuffer::zero().clear(0.f, 0);

        controller.update(camera, window, window.delta_time());
        data_buffer[0].projection_mat = camera.projection();
        data_buffer[0].view_mat = camera.view();
        data_buffer.synchronize();

        model_buffer[0].model_mat = glm::rotate(glm::radians(-90.f), glm::vec3(1, 0, 0));
        model_buffer[0].normal_mat = inverse(transpose(model_buffer[0].model_mat));
        model_buffer.synchronize();

        ship_pipeline.bind();
        ship_pipeline.bind_attribute(0, ship_vbo, 3, GL_FLOAT, offsetof(res::vertex, position), sizeof(res::vertex));
        ship_pipeline.bind_attribute(1, ship_vbo, 3, GL_FLOAT, offsetof(res::vertex, normal), sizeof(res::vertex));
        ship_pipeline.bind_attribute(2, ship_vbo, 2, GL_FLOAT, offsetof(res::vertex, uv), sizeof(res::vertex));
        ship_pipeline.bind_uniform_buffer(0, data_buffer);
        ship_pipeline.bind_uniform_buffer(1, model_buffer);
        ship_pipeline.draw(GL_TRIANGLES, ship_ibo, GL_UNSIGNED_INT);

        model_buffer[0].model_mat = glm::mat4(1.f);
        model_buffer[0].normal_mat = inverse(transpose(model_buffer[0].model_mat));
        model_buffer.synchronize();

        ship_pipeline.bind();
        ship_pipeline.bind_attribute(0, sail_vertices, 3, GL_FLOAT, offsetof(res::vertex, position), sizeof(res::vertex));
        ship_pipeline.bind_attribute(1, sail_vertices, 3, GL_FLOAT, offsetof(res::vertex, normal), sizeof(res::vertex));
        ship_pipeline.bind_attribute(2, sail_vertices, 2, GL_FLOAT, offsetof(res::vertex, uv), sizeof(res::vertex));
        ship_pipeline.bind_uniform_buffer(0, data_buffer);
        ship_pipeline.bind_uniform_buffer(1, model_buffer);
        ship_pipeline.draw(GL_TRIANGLES, sail_indices, GL_UNSIGNED_INT);

        static bool integ = false;

        if (integ)
        {
            float fac = 1.f;
            if ((frame = std::min(frame+1, 3)) == 2)
                fac = 2.f;
            sail_timings_buffer[0].delta = window.delta_time();
            sail_timings_buffer[0].deltav = window.delta_time() / fac;
            sail_timings_buffer.synchronize();

            sail_spring_pipeline.bind_uniform_buffer(0, sail_sim_data_buffer);
            sail_spring_pipeline.dispatch(springs.size());
            glFinish();
            sail_integrate_pipeline.bind_uniform_buffer(0, sail_sim_data_buffer);
            sail_integrate_pipeline.bind_uniform_buffer(1, sail_timings_buffer);
            sail_integrate_pipeline.dispatch(sail_vertices.size());
            glFinish();
            vertex_springs.clear(0);
        }

        ImGui::Begin("Settings");
        ImGui::Checkbox("Integ", &integ);
        ImGui::End();
    }

    return 0;
}