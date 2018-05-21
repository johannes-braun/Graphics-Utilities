#include <opengl/pipeline.hpp>
#include <opengl/framebuffer.hpp>

#include <gfx/file.hpp>
#include <gfx/geometry.hpp>
#include <gfx/window.hpp>
#include <gfx/imgui.hpp>
#include <gfx/log.hpp>
#include <gfx/camera.hpp>

struct spring
{
    int particle1;
    int particle2;
    float stiff;
    float damping;
    float length;
    float _p[3];
};

int main()
{
    gl::shader::set_include_directories(std::vector<gfx::files::path>{ "../shd", SOURCE_DIRECTORY "/global/shd" });

    gfx::window_hints hints;
    hints[GLFW_SAMPLES] = 8;
    auto window = std::make_shared<gfx::window>(gfx::apis::opengl::name, "Ship Sim", 1280, 720, hints);
    gfx::imgui imgui(window);
    gfx::camera camera;
    gfx::camera_controller controller(window);

    gfx::scene_file ship_file("ship.dae");
    gl::buffer<gfx::vertex3d> ship_vbo(ship_file.meshes.begin()->vertices.begin(), ship_file.meshes.begin()->vertices.end());
    gl::buffer<gfx::index32> ship_ibo(ship_file.meshes.begin()->indices.begin(), ship_file.meshes.begin()->indices.end());

    gl::pipeline ship_pipeline;
    ship_pipeline[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("ship.vert");
    ship_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("ship.frag");

    struct data { glm::mat4 view_mat, projection_mat; };
    struct model { glm::mat4 model_mat, normal_mat; glm::vec4 color; uintptr_t tex; uint32_t has_texture; };
    gl::buffer<data> data_buffer(1, GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<model> model_buffer(1, GL_DYNAMIC_STORAGE_BIT);

    constexpr int size = 10;
    constexpr int fsize = 5;
    gl::buffer<gfx::vertex3d> sail_vertices(size * size + fsize * fsize, GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<gfx::index32> sail_indices((size - 1) * (size - 1) * 6 + (fsize-1) * (fsize-1) * 6, GL_DYNAMIC_STORAGE_BIT);

    gfx::image_file sail_image("sail.jpg", gfx::bits::b8, 4);
    gl::texture sail(GL_TEXTURE_2D, sail_image.width, sail_image.height, GL_RGBA8);
    sail.assign(GL_RGBA, GL_UNSIGNED_BYTE, sail_image.bytes());
    sail.generate_mipmaps();
    const gl::sampler sampler;
    
    int i=0; 
    for (int y = 0; y < size; ++y) for (int x = 0; x < size; ++x)
    {
        const glm::vec2 uv{ x / float(size-1), y / float(size-1) };
        sail_vertices[y * size + x].uv = uv;
        sail_vertices[y * size + x].position = { uv.x * 3.6f-1.8f, uv.y*2.31257f + 1.55686f, 0.438 };
        sail_vertices[y * size + x].normal = { 0, 0, 1 };
        sail_vertices[y * size + x].metadata_position = 0;
        sail_vertices[y * size + x].metadata_normal = (y == 0 || y == size-1) && x%3==0;
        sail_vertices[y * size + x].metadata_uv = 0ll;

        if (x < size - 1 && y < size - 1)
        {
            sail_indices[i++] = y * size + x;
            sail_indices[i++] = (y + 1)*size + x;
            sail_indices[i++] = y * size + x + 1;

            sail_indices[i++] = y * size + x + 1;
            sail_indices[i++] = (y + 1)*size + x;
            sail_indices[i++] = (y + 1)*size + x + 1;
        }
    }

    for (int y = 0; y < fsize; ++y) for (int x = 0; x < fsize; ++x)
    {
        const glm::vec2 uv{ x / float(fsize - 1), y / float(fsize - 1) };
        sail_vertices[size * size + y * fsize + x].uv = uv;
        sail_vertices[size * size + y * fsize + x].position = { uv.x, 0.33f*uv.y + 4.6f, uv.x+ 0.428 };
        sail_vertices[size * size + y * fsize + x].normal = { 1, 0, 0 };
        sail_vertices[size * size + y * fsize + x].metadata_position = 0;
        sail_vertices[size * size + y * fsize + x].metadata_normal = (x == 0) && x % 2 == 0;
        sail_vertices[size * size + y * fsize + x].metadata_uv = 0ll;

        if (x < fsize - 1 && y < fsize - 1)
        {
            sail_indices[i++] = size * size + y * fsize + x;
            sail_indices[i++] = size * size + (y + 1)*fsize + x;
            sail_indices[i++] = size * size + y * fsize + x + 1;

            sail_indices[i++] = size * size + y * fsize + x + 1;
            sail_indices[i++] = size * size + (y + 1)*fsize + x;
            sail_indices[i++] = size * size + (y + 1)*fsize + x + 1;
        }
    }

    sail_vertices.synchronize();
    sail_indices.synchronize();

    gl::buffer<spring> springs(GL_DYNAMIC_STORAGE_BIT);
    for (int s = 0; s < size*size; ++s) for (int e = s + 1; e < size * size; ++e)
    {
        const glm::vec3 p1 = sail_vertices[s].position;
        const glm::vec3 p2 = sail_vertices[e].position;

        glm::vec2 scaled_distance = abs(glm::vec2(p2 - p1));
        scaled_distance.x /= 3.6f / (size - 1);
        scaled_distance.y /= 2.31257f / (size - 1);

        if (length(scaled_distance) < 2.01f)
            springs.push_back(spring{ s, e, 100.f, 0.2f, distance(p1, p2) });
    }

    for (int s = 0; s < fsize*fsize; ++s) for (int e = s + 1; e < fsize * fsize; ++e)
    {
        const glm::vec3 p1 = sail_vertices[size * size + s].position;
        const glm::vec3 p2 = sail_vertices[size * size + e].position;

        glm::vec3 scaled_distance = abs(glm::vec3(p2 - p1));
        scaled_distance.x = 0;
        scaled_distance.y /= 0.33f / (fsize - 1);
        scaled_distance.z /= 1.f / (fsize - 1);

        if (length(scaled_distance) < 2.01f)
            springs.push_back(spring{ size * size + s, size * size + e, 100.f, 0.1f, distance(p1, p2) });
    }

    gfx::transform ship_transform;

    struct sail_sim_data { 
        uintptr_t spring_results;
        uintptr_t particle_velocities;
        uintptr_t springs;
        uintptr_t vertices;
        uintptr_t vertex_springs; 
        int spring_count;
        int vertex_count;
        glm::mat4 inv_model;
    };

    struct normal_data {
        uintptr_t vertices;
        uintptr_t indices;
        int triangle_count;
    };

    struct timings
    {
        float delta;
        float deltav;
        float time;
    };

    gl::compute_pipeline sail_spring_pipeline(std::make_shared<gl::shader>("sails/spring.comp"));
    gl::compute_pipeline sail_integrate_pipeline(std::make_shared<gl::shader>("sails/integrate.comp"));
    gl::compute_pipeline sail_normals_pipeline(std::make_shared<gl::shader>("sails/normals.comp"));

    gl::buffer<sail_sim_data> sail_sim_data_buffer(1, GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<normal_data> sail_normal_data_buffer(1, GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<timings> sail_timings_buffer(1, GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<glm::vec4> spring_results((12) * sail_vertices.size(), GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<uint32_t> vertex_springs(sail_vertices.size(), GL_DYNAMIC_STORAGE_BIT);
    gl::buffer<glm::vec4> sail_velocities(sail_vertices.size(), GL_DYNAMIC_STORAGE_BIT);
    sail_sim_data_buffer[0].particle_velocities = sail_velocities.handle();
    sail_sim_data_buffer[0].spring_results = spring_results.handle();
    sail_sim_data_buffer[0].springs = springs.handle();
    sail_sim_data_buffer[0].vertices = sail_vertices.handle();
    sail_sim_data_buffer[0].vertex_springs = vertex_springs.handle();
    sail_sim_data_buffer[0].spring_count = static_cast<int>(springs.size());
    sail_sim_data_buffer[0].vertex_count = static_cast<int>(sail_vertices.size());
    sail_sim_data_buffer.synchronize();

    glEnable(GL_DEPTH_TEST);
    int frame = 0;
    while (window->update())
    {
        imgui.new_frame();
        gl::framebuffer::zero().clear(0, { 0.5f, 0.9f, 1.f, 1.f });
        gl::framebuffer::zero().clear(0.f, 0);

        ship_transform.position += -ship_transform.forward() * 4.f * window->delta_time();
        ship_transform.rotation *= glm::angleAxis(glm::radians(40.f * float(window->delta_time())), glm::vec3(0, 1, 0));

        controller.update(camera);
        data_buffer[0].projection_mat = camera.projection();
        data_buffer[0].view_mat = camera.view();
        data_buffer.synchronize();

        model_buffer[0].model_mat = glm::mat4(ship_transform) * glm::rotate(glm::radians(-90.f), glm::vec3(1, 0, 0));
        model_buffer[0].normal_mat = inverse(transpose(model_buffer[0].model_mat));
        model_buffer[0].color = glm::vec4(0.5f, 0.3f, 0.1f, 1.f);
        model_buffer[0].tex = 0;
        model_buffer[0].has_texture = false;
        model_buffer.synchronize();

        ship_pipeline.bind();
        ship_pipeline.bind_attribute(0, ship_vbo, 3, GL_FLOAT, offsetof(gfx::vertex3d, position), sizeof(gfx::vertex3d));
        ship_pipeline.bind_attribute(1, ship_vbo, 3, GL_FLOAT, offsetof(gfx::vertex3d, normal), sizeof(gfx::vertex3d));
        ship_pipeline.bind_attribute(2, ship_vbo, 2, GL_FLOAT, offsetof(gfx::vertex3d, uv), sizeof(gfx::vertex3d));
        ship_pipeline.bind_uniform_buffer(0, data_buffer);
        ship_pipeline.bind_uniform_buffer(1, model_buffer);
        ship_pipeline.draw(GL_TRIANGLES, ship_ibo, GL_UNSIGNED_INT);

        model_buffer[0].model_mat = glm::mat4(ship_transform);
        model_buffer[0].normal_mat = inverse(transpose(model_buffer[0].model_mat));
        model_buffer[0].color = glm::vec4(0.8f);
        model_buffer[0].tex = sampler.sample(sail);
        model_buffer[0].has_texture = true;
        model_buffer.synchronize();

        ship_pipeline.bind();
        ship_pipeline.bind_attribute(0, sail_vertices, 3, GL_FLOAT, offsetof(gfx::vertex3d, position), sizeof(gfx::vertex3d));
        ship_pipeline.bind_attribute(1, sail_vertices, 3, GL_FLOAT, offsetof(gfx::vertex3d, normal), sizeof(gfx::vertex3d));
        ship_pipeline.bind_attribute(2, sail_vertices, 2, GL_FLOAT, offsetof(gfx::vertex3d, uv), sizeof(gfx::vertex3d));
        ship_pipeline.bind_uniform_buffer(0, data_buffer);
        ship_pipeline.bind_uniform_buffer(1, model_buffer);
        ship_pipeline.draw(GL_TRIANGLES, sail_indices, GL_UNSIGNED_INT);

        static bool integ = false;

        if (integ)
        {
            float fac = 1.f;
            if ((frame = std::min(frame+1, 3)) == 2)
                fac = 2.f;
            sail_timings_buffer[0].delta = static_cast<float>(window->delta_time());
            sail_timings_buffer[0].deltav = static_cast<float>(window->delta_time() / fac);
            sail_timings_buffer[0].time = static_cast<float>(glfwGetTime());
            sail_timings_buffer.synchronize();

            sail_normal_data_buffer[0].vertices = sail_vertices.handle();
            sail_normal_data_buffer[0].indices = sail_indices.handle();
            sail_normal_data_buffer[0].triangle_count = static_cast<int>(sail_indices.size() / 3);
            sail_normal_data_buffer.synchronize();

            sail_sim_data_buffer[0].inv_model = inverse(glm::mat4(ship_transform));
            sail_sim_data_buffer.synchronize();

            sail_spring_pipeline.bind_uniform_buffer(0, sail_sim_data_buffer);
            sail_spring_pipeline.dispatch(static_cast<uint32_t>(springs.size()));
            glFinish();
            sail_integrate_pipeline.bind_uniform_buffer(0, sail_sim_data_buffer);
            sail_integrate_pipeline.bind_uniform_buffer(1, sail_timings_buffer);
            sail_integrate_pipeline.dispatch(static_cast<uint32_t>(sail_vertices.size()));
            glFinish();
            sail_normals_pipeline.bind_uniform_buffer(0, sail_normal_data_buffer);
            sail_normals_pipeline.dispatch(sail_normal_data_buffer[0].triangle_count);
            glFinish();
            vertex_springs.clear(0);
        }

        ImGui::Begin("Settings");
        ImGui::Checkbox("Integ", &integ);
        ImGui::Value("DT", float(window->delta_time()));
        if (ImGui::Button("Print"))
        {
            for (auto&& vert : sail_vertices)
                log_i << to_string(vert.normal);
        }
        if (ImGui::Button("Reload"))
        {
            sail_spring_pipeline.reload();
            sail_integrate_pipeline.reload();
            sail_normals_pipeline.reload();
        }
        ImGui::End();

        imgui.render();
    }

    return 0;
}