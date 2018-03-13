#include <jpu/memory.hpp>
#include <jpu/data.hpp>
#include "io/window.hpp"
#include "io/camera.hpp"
#include "stb_image.h"
#include "stb_image_write.h"
#include "res/image.hpp"
#include "res/presets.hpp"
#include "tinyfd/tinyfiledialogs.h"

#include "eigen3/Eigen/Eigenvalues"
#include <opengl/framebuffer.hpp>
#include <numeric>

jpu::ref_ptr<io::window> main_window;
jpu::named_vector<std::string, jpu::ref_ptr<gl::graphics_pipeline>> graphics_pipelines;

struct axis_transformation
{
    glm::mat4 basic_rgb;
    glm::mat4 eigen_rgb;

    glm::mat4 basic_pomierski;
    glm::mat4 eigen_pomierski;
};

struct eigenresults
{
    glm::dmat3 eigenvectors;
    glm::dvec3 eigenvalues;
};

eigenresults compute_eigen(const glm::dmat3& matrix)
{
    static Eigen::EigenSolver<Eigen::Matrix3d> solver;
    Eigen::Matrix3d matrix_eigen = reinterpret_cast<const Eigen::Matrix3d&>(matrix);
    solver.compute(matrix_eigen);

    eigenresults results;
    Eigen::Matrix3d vectors = solver.eigenvectors().real();
    Eigen::Vector3d values = solver.eigenvalues().real();
    results.eigenvalues = reinterpret_cast<const glm::dvec3&>(values);
    results.eigenvectors = reinterpret_cast<const glm::dmat3&>(vectors);
    return results;
}

axis_transformation principal_axis_transformation(glm::u8vec3* pixels, const size_t count)
{
    const double color_to_float = 1.0 / 255.0;
    const glm::dvec3 main_axis_rgb = glm::normalize(glm::dvec3(1.0));
    const glm::dvec3 main_axis_pomierski = glm::dvec3(0, 0, 1);
    const glm::dmat3 pomierski{
        0.5,   -0.875,  1.5,
       -1.0,    0,      1.5,
        0.5,    0.875,  1.5
    };
    const glm::dmat3 pomierski_inverse = inverse(pomierski);

    const glm::dvec3 average_rgb = std::accumulate(pixels, pixels + count, glm::dvec3(0.0), [color_to_float](const glm::dvec3& lhs, const glm::u8vec3& rhs) {
        return lhs + glm::dvec3(rhs) * color_to_float;
    }) / static_cast<double>(count);
    const glm::dvec3 average_pomierski = pomierski * average_rgb;

    const glm::dmat4 translate_rgb = glm::translate(-average_rgb);
    const glm::dmat4 translate_back_rgb = glm::translate(main_axis_rgb * length(average_rgb));
    const glm::dmat4 translate_pomierski = glm::translate(-average_pomierski);
    const glm::dmat4 translate_back_pomierski = glm::translate(main_axis_pomierski * length(average_pomierski));

    const glm::dmat3 covariance_rgb = std::accumulate(pixels, pixels + count, glm::dmat3(0.0), [color_to_float, average_rgb](const glm::dmat3& lhs, const glm::u8vec3& rhs) {
        const glm::dvec3 error = glm::dvec3(rhs) * color_to_float - average_rgb;
        return lhs + outerProduct(error, error);
    }) / static_cast<double>(count);
    const glm::dmat3 covariance_pomierski = pomierski * covariance_rgb * transpose(pomierski);

    const eigenresults eigen_rgb = compute_eigen(covariance_rgb);
    const eigenresults eigen_pomierski = compute_eigen(covariance_pomierski);

    const auto get_axis_basic = [](const glm::dmat3& covariance) {
        double length = glm::length(covariance[0]);
        int current = 0;
        if (glm::length(covariance[1]) > length)
            current = (length = glm::length(covariance[1]), 1);
        if (glm::length(covariance[2]) > length)
            current = (length = glm::length(covariance[2]), 2);
        return covariance[current] / length;
    };

    const auto get_axis_eigen = [](const eigenresults& eigen) {
        double length = eigen.eigenvalues[0];
        int current = 0;
        if (eigen.eigenvalues[1] > length)
            current = (length = eigen.eigenvalues[1], 1);
        if (eigen.eigenvalues[2] > length)
            current = (length = eigen.eigenvalues[2], 2);
        return eigen.eigenvectors[current];
    };

    const glm::dvec3 axis_basic_rgb = get_axis_basic(covariance_rgb);
    const glm::dvec3 axis_eigen_rgb = get_axis_eigen(eigen_rgb);
    const glm::dvec3 axis_basic_pomierski = get_axis_basic(covariance_pomierski);
    const glm::dvec3 axis_eigen_pomierski = get_axis_eigen(eigen_pomierski);

    const auto make_rotation = [](const glm::dvec3& axis, const glm::dvec3& main_axis) {
        const double angle = dot(axis, main_axis);
        return toMat4(angleAxis(abs(angle), (angle > 0) ? cross(axis, main_axis) : cross(-axis, main_axis)));
    };

    axis_transformation result;
    result.basic_rgb = translate_back_rgb * make_rotation(axis_basic_rgb, main_axis_rgb) * translate_rgb;
    result.eigen_rgb = translate_back_rgb * make_rotation(axis_eigen_rgb, main_axis_rgb) * translate_rgb;
    result.basic_pomierski = glm::dmat4(pomierski_inverse) * translate_back_pomierski *
        make_rotation(axis_basic_pomierski, main_axis_pomierski) * translate_pomierski * glm::dmat4(pomierski);
    result.eigen_pomierski = glm::dmat4(pomierski_inverse) * translate_back_pomierski *
        make_rotation(axis_eigen_pomierski, main_axis_pomierski) * translate_pomierski * glm::dmat4(pomierski);

    return result;
}

int main()
{
    gl::shader::set_include_directories("../shaders");

    res::image icon = load_image("../res/ui/logo.png", res::image_type::u8, res::image_components::rgb_alpha);
    res::image cursor = load_image("../res/cursor.png", res::image_type::u8, res::image_components::rgb_alpha);

    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    main_window = jpu::make_ref<io::window>(io::api::opengl, 1280, 720, "My Window");
    main_window->set_icon(icon.width, icon.height, icon.data.get());
    main_window->set_cursor(new io::cursor(cursor.width, cursor.height, cursor.data.get(), 0, 0));
    main_window->set_max_framerate(60.0);
    main_window->callbacks->key_callback.add([](GLFWwindow*, int key, int, int action, int mods) {
        if (action == GLFW_PRESS && key == GLFW_KEY_P)
            for (auto&& p : graphics_pipelines)
                p->reload_stages();
    });

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    constexpr const char *fs[2] = { "*.jpg", "*.png" };
    const auto src_data = tinyfd_openFileDialog("Open Image", "../res/", 2, fs, "Images", false);
    if (!src_data) return 0;

    res::image picture = res::load_image(src_data, res::image_type::u8, res::image_components::rgb);
    gl::texture texture(GL_TEXTURE_2D, picture.width, picture.height, GL_RGB8);
    texture.assign(GL_RGB, GL_UNSIGNED_BYTE, picture.data.get());
    texture.generate_mipmaps();
    const gl::sampler sampler;

    glm::u8vec3* begin = reinterpret_cast<glm::u8vec3*>(picture.data.get());
    axis_transformation trafo = principal_axis_transformation(begin, picture.num_pixels());
    const glm::vec3 average = glm::vec3(std::accumulate(begin, begin + picture.num_pixels(), glm::u8vec3(0))) / (picture.num_pixels() * 255.f);

    auto grid_image = load_image("../res/grid.jpg", res::image_type::u8, res::image_components::rgb_alpha);
    gl::texture grid(GL_TEXTURE_2D, grid_image.width, grid_image.height, GL_RGBA8);
    grid.assign(GL_RGBA, GL_UNSIGNED_BYTE, grid_image.data.get());
    grid.generate_mipmaps();

    auto points_pipeline = graphics_pipelines.push("Points Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    points_pipeline->use_stages(std::make_shared<gl::shader>("color_space/points.vert"), std::make_shared<gl::shader>("color_space/points.frag"));

    auto center_pipeline = graphics_pipelines.push("Center Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    center_pipeline->use_stages(std::make_shared<gl::shader>("color_space/center_point.vert"), std::make_shared<gl::shader>("color_space/center_point.frag"));

    auto gizmo_pipeline = graphics_pipelines.push("Gizmo Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    gizmo_pipeline->use_stages(std::make_shared<gl::shader>("color_space/gizmo.vert"), std::make_shared<gl::shader>("color_space/gizmo.frag"));

    auto img_pipeline = graphics_pipelines.push("Image Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    img_pipeline->use_stages(std::make_shared<gl::shader>("postprocess/screen.vert"), std::make_shared<gl::shader>("color_space/image.frag"));

    auto cube_pipeline = graphics_pipelines.push("Cube Pipeline", jpu::make_ref<gl::graphics_pipeline>());
    cube_pipeline->use_stages(std::make_shared<gl::shader>("color_space/cube.vert"), std::make_shared<gl::shader>("color_space/cube.frag"));
    cube_pipeline->set_input_format(0, 3, GL_FLOAT, false);
    cube_pipeline->set_input_format(1, 2, GL_FLOAT, false);

    namespace cube = res::presets::cube;
    gl::buffer<res::vertex> vbo(cube::vertices.begin(), cube::vertices.end());
    gl::buffer<uint32_t> ibo(cube::indices.begin(), cube::indices.end());

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glLineWidth(4.f);

    io::camera cam;
    io::default_cam_controller cam_controller;
    cam.transform.position = glm::vec3(0, 0, 5);

    while (main_window->update())
    {
        gl::framebuffer::zero().clear(0, { 0.15f, 0.15f, 0.15f, 1.f });
        gl::framebuffer::zero().clear(0.f, 0);

        const auto id = sampler.sample(texture);
        ImGui::Begin("Primary Axis Transformation");
        static bool hat_en = true;
        ImGui::Checkbox("Enable Transformation", &hat_en);

        glm::mat4 patmat = glm::mat4(1.f);
        if (hat_en)
        {
            static int mode = 0;
            ImGui::Combo("Mode", &mode, " Basic (RGB) \0 Eigen (RGB) \0 Basic (POM) \0 Eigen (POM) \0");

            switch (mode)
            {
            case 0: patmat = trafo.basic_rgb; break;
            case 1: patmat = trafo.eigen_rgb; break;
            case 2: patmat = trafo.basic_pomierski; break;
            case 3: patmat = trafo.eigen_pomierski; break;
            default:;
            }
        }

        static float scale = 1.f;
        ImGui::DragFloat("Preview Scale", &scale, 0.01f, 0.f, 100.f);

        if (ImGui::Button("Load", ImVec2(ImGui::GetContentRegionAvailWidth() * 0.5f, 0)))
        {
            if (const auto src = tinyfd_openFileDialog("Open Image", "../res/", 2, fs, "Images", false))
            {
                picture = res::load_image(src_data, res::image_type::u8, res::image_components::rgb);
                texture = gl::texture(GL_TEXTURE_2D, picture.width, picture.height, GL_RGB8);
                texture.assign(GL_RGB, GL_UNSIGNED_BYTE, picture.data.get());
                texture.generate_mipmaps();

                glm::u8vec3* begin = reinterpret_cast<glm::u8vec3*>(picture.data.get());
                axis_transformation trafo = principal_axis_transformation(begin, picture.num_pixels());
                const glm::vec3 average = glm::vec3(std::accumulate(begin, begin + picture.num_pixels(), glm::u8vec3(0))) / (picture.num_pixels() * 255.f);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
        {
            glm::u8vec3* data_cast = reinterpret_cast<glm::u8vec3*>(picture.data.get());
            std::vector<glm::u8vec3> new_img(picture.num_pixels());

#pragma omp parallel for schedule(static)
            for (int p = 0; p < picture.num_pixels(); ++p)
                new_img[p] = clamp(glm::vec3(patmat * glm::vec4(glm::vec3(data_cast[p]) / 255.f, 1)) * 255.f, glm::vec3(0), glm::vec3(255.f));

            constexpr const char *ext[1] = { "*.png" };
            if (const auto dst = tinyfd_saveFileDialog("Save output", "../res/", 1, ext, "PNG"))
                stbi_write_png(dst, picture.width, picture.height, 3, new_img.data(), 0);
        }
        ImGui::End();

        cam_controller.update(cam, *main_window, main_window->delta_time());

        points_pipeline->bind();
        points_pipeline->get_uniform<glm::mat4>(GL_VERTEX_SHADER, "hat_mat") = hat_en ? patmat : glm::mat4(1.0);
        points_pipeline->get_uniform<uint64_t>(GL_VERTEX_SHADER, "picture") = id;
        points_pipeline->get_uniform<glm::mat4>(GL_VERTEX_SHADER, "view_projection") = cam.projection() * cam.view();
        points_pipeline->draw(gl::primitive::points, texture.width() * texture.height());

        glDisable(GL_DEPTH_TEST);
        center_pipeline->bind();
        center_pipeline->get_uniform<glm::mat4>(GL_VERTEX_SHADER, "hat_mat") = hat_en ? patmat : glm::mat4(1.0);
        center_pipeline->get_uniform<glm::mat4>(GL_VERTEX_SHADER, "view_projection") = cam.projection() * cam.view();
        center_pipeline->get_uniform<glm::vec3>(GL_VERTEX_SHADER, "center") = average;
        center_pipeline->draw(gl::primitive::points, 1);
        glEnable(GL_DEPTH_TEST);

        gizmo_pipeline->bind();
        gizmo_pipeline->get_uniform<glm::mat4>(GL_VERTEX_SHADER, "view_projection") = cam.projection() * cam.view();
        gizmo_pipeline->draw(gl::primitive::lines, 6);

        glFrontFace(GL_CW);
        cube_pipeline->bind();
        cube_pipeline->set_input_buffer(0, vbo, sizeof(res::vertex), offsetof(res::vertex, position));
        cube_pipeline->set_input_buffer(1, vbo, sizeof(res::vertex), offsetof(res::vertex, uv));
        cube_pipeline->set_index_buffer(ibo, gl::index_type::u32);
        cube_pipeline->get_uniform<glm::mat4>(GL_VERTEX_SHADER, "view_projection") = cam.projection() * cam.view();
        cube_pipeline->get_uniform<uint64_t>(GL_FRAGMENT_SHADER, "tex") = sampler.sample(grid);
        cube_pipeline->get_uniform<glm::vec4>(GL_FRAGMENT_SHADER, "tint") = glm::vec4(1, 1, 1, 1);
        cube_pipeline->draw_indexed(gl::primitive::triangles, res::presets::cube::indices.size());
        glFrontFace(GL_CCW); glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        cube_pipeline->get_uniform<glm::vec4>(GL_FRAGMENT_SHADER, "tint") = glm::vec4(1, 1, 1, -1);
        cube_pipeline->draw_indexed(gl::primitive::triangles, res::presets::cube::indices.size());
        glDisable(GL_BLEND);

        glDisable(GL_DEPTH_TEST);
        glViewportIndexedf(0, 0, 0, static_cast<int>(texture.width() *scale), static_cast<int>(texture.height() *scale));
        img_pipeline->bind();
        img_pipeline->get_uniform<glm::mat4>(GL_FRAGMENT_SHADER, "hat_mat") = hat_en ? patmat : glm::mat4(1.0);
        img_pipeline->get_uniform<uint64_t>(GL_FRAGMENT_SHADER, "tex") = id;
        img_pipeline->draw(gl::primitive::triangles, 3);

        int w, h;
        glfwGetFramebufferSize(*main_window, &w, &h);
        glViewportIndexedf(0, 0, 0, w, h);
        glEnable(GL_DEPTH_TEST);
    }
}