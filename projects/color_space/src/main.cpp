#include "eigen3/Eigen/Eigenvalues"
#include <opengl/framebuffer.hpp>
#include <numeric>

#include <gfx/camera.hpp>
#include <gfx/geometry.hpp>
#include <gfx/file.hpp>
#include <gfx/window.hpp>
#include <gfx/imgui.hpp>

#include <imgui/imgui.h>

#include <opengl/pipeline.hpp>
#include <opengl/shader.hpp>
#include <opengl/buffer.hpp>

std::shared_ptr<gfx::window> main_window;

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
    gl::shader::set_include_directories(std::vector<gfx::files::path>{ "../shd", SOURCE_DIRECTORY "/global/shd" });

    gfx::window_hints hints;
    hints[GLFW_SAMPLES] = 8;
    hints[GLFW_OPENGL_DEBUG_CONTEXT] = true;
    main_window = std::make_shared<gfx::window>(gfx::apis::opengl::name, "My Window", 1280, 720, hints);
    main_window->set_icon(gfx::image_file("ui/logo.png", gfx::bits::b8, 4));
    main_window->set_max_framerate(60.0);

    gfx::imgui imgui(main_window);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    const auto source_data = gfx::file::open_dialog("Open Image", "../", { "*.jpg", "*.png" }, "Image Files");
    if (!source_data) return 0;

    gfx::image_file picture(*source_data, gfx::bits::b8, 3);
    gl::texture texture(GL_TEXTURE_2D, picture.width, picture.height, GL_RGB8);
    texture.assign(GL_RGB, GL_UNSIGNED_BYTE, picture.bytes());
    texture.generate_mipmaps();
    const gl::sampler sampler;

    glm::u8vec3* begin = reinterpret_cast<glm::u8vec3*>(picture.bytes());
    axis_transformation trafo = principal_axis_transformation(begin, picture.pixel_count());
    glm::vec3 average = glm::vec3(std::accumulate(begin, begin + picture.pixel_count(), glm::u8vec3(0))) / (picture.pixel_count() * 255.f);

    gfx::image_file grid_image("grid.jpg", gfx::bits::b8, 4);
    gl::texture grid(GL_TEXTURE_2D, grid_image.width, grid_image.height, GL_RGBA8);
    grid.assign(GL_RGBA, GL_UNSIGNED_BYTE, grid_image.bytes());
    grid.generate_mipmaps();

    gl::pipeline points_pipeline;
    points_pipeline[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("points.vert");
    points_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("points.frag");

    gl::pipeline center_pipeline;
    center_pipeline[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("center_point.vert");
    center_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("center_point.frag");

    gl::pipeline gizmo_pipeline;
    gizmo_pipeline[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("gizmo.vert");
    gizmo_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("gizmo.frag");

    gl::pipeline img_pipeline;
    img_pipeline[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("postfx/screen.vert");
    img_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("image.frag");

    gl::pipeline cube_pipeline;
    cube_pipeline[GL_VERTEX_SHADER] = std::make_shared<gl::shader>("cube.vert");
    cube_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("cube.frag");
    
    namespace cube = gfx::cube_preset;
    gl::buffer<gfx::vertex> vbo(cube::vertices.begin(), cube::vertices.end());
    gl::buffer<gfx::index32> ibo(cube::indices.begin(), cube::indices.end());

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glLineWidth(4.f);

    gfx::camera camera;
    camera.transform.position = glm::vec3(0, 0, 5);
    gfx::camera_controller controller(main_window);

    while (main_window->update())
    {
        imgui.new_frame();

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
            if (const auto source_data = gfx::file::open_dialog("Open Image", "../", { "*.jpg", "*.png" }, "Image Files");)
            {
                gfx::image_file picture(*source_data, gfx::bits::b8, 3);
                texture = gl::texture(GL_TEXTURE_2D, picture.width, picture.height, GL_RGB8);
                texture.assign(GL_RGB, GL_UNSIGNED_BYTE, picture.bytes());
                texture.generate_mipmaps();

                glm::u8vec3* begin = reinterpret_cast<glm::u8vec3*>(picture.bytes());
                trafo = principal_axis_transformation(begin, picture.pixel_count());
                average = glm::vec3(std::accumulate(begin, begin + picture.pixel_count(), glm::u8vec3(0))) / (picture.pixel_count() * 255.f);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
        {
            glm::u8vec3* data_cast = static_cast<glm::u8vec3*>(picture.bytes());
            std::vector<glm::u8vec3> new_img(picture.pixel_count());
            std::transform(data_cast, data_cast + picture.pixel_count(), new_img.begin(), [&patmat](const glm::u8vec3& vec) {
                return clamp(glm::vec3(patmat * glm::vec4(glm::vec3(vec) / 255.f, 1)) * 255.f, glm::vec3(0), glm::vec3(255.f));
            });

            if (const auto dst = gfx::file::save_dialog("Save output", "../", { "*.png" }, "PNG"))
                gfx::image_file::save_png(*dst, picture.width, picture.height, 3, &new_img[0][0]);
        }
        ImGui::End();

        controller.update(camera);

        points_pipeline.bind();
        points_pipeline[GL_VERTEX_SHADER]->uniform<glm::mat4>("hat_mat") = hat_en ? patmat : glm::mat4(1.0);
        points_pipeline[GL_VERTEX_SHADER]->uniform<uint64_t>("picture") = id;
        points_pipeline[GL_VERTEX_SHADER]->uniform<glm::mat4>("view_projection") = camera.projection() * camera.view();
        points_pipeline.draw(GL_POINTS, texture.width() * texture.height());

        glDisable(GL_DEPTH_TEST);
        center_pipeline.bind();
        center_pipeline[GL_VERTEX_SHADER]->uniform<glm::mat4>("hat_mat") = hat_en ? patmat : glm::mat4(1.0);
        center_pipeline[GL_VERTEX_SHADER]->uniform<glm::mat4>("view_projection") = camera.projection() * camera.view();
        center_pipeline[GL_VERTEX_SHADER]->uniform<glm::vec3>("center") = average;
        center_pipeline.draw(GL_POINTS, 1);
        glEnable(GL_DEPTH_TEST);

        gizmo_pipeline.bind();
        gizmo_pipeline[GL_VERTEX_SHADER]->uniform<glm::mat4>("view_projection") = camera.projection() * camera.view();
        gizmo_pipeline.draw(GL_LINES, 6);

        glFrontFace(GL_CW);

        cube_pipeline.bind();
        cube_pipeline.bind_attribute(0, vbo, 3, GL_FLOAT, offsetof(gfx::vertex, position));
        cube_pipeline.bind_attribute(1, vbo, 2, GL_FLOAT, offsetof(gfx::vertex, uv));

        cube_pipeline[GL_VERTEX_SHADER]->uniform<glm::mat4>("view_projection") = camera.projection() * camera.view();
        cube_pipeline[GL_FRAGMENT_SHADER]->uniform<uint64_t>("tex") = sampler.sample(grid);
        cube_pipeline[GL_FRAGMENT_SHADER]->uniform<glm::vec4>("tint") = glm::vec4(1, 1, 1, 1);
        cube_pipeline.draw(GL_TRIANGLES, ibo, GL_UNSIGNED_INT);

        glFrontFace(GL_CCW); glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        cube_pipeline[GL_FRAGMENT_SHADER]->uniform<glm::vec4>("tint") = glm::vec4(1, 1, 1, -1);
        cube_pipeline.draw(GL_TRIANGLES, ibo, GL_UNSIGNED_INT);
        glDisable(GL_BLEND);

        glDisable(GL_DEPTH_TEST);
        glViewportIndexedf(0, 0, 0, texture.width() *scale, texture.height() *scale);
        img_pipeline.bind();
        img_pipeline[GL_FRAGMENT_SHADER]->uniform<glm::mat4>("hat_mat") = hat_en ? patmat : glm::mat4(1.0);
        img_pipeline[GL_FRAGMENT_SHADER]->uniform<uint64_t>("tex") = id;
        img_pipeline.draw(GL_TRIANGLES, 3);

        int w, h;
        glfwGetFramebufferSize(*main_window, &w, &h);
        glViewportIndexedf(0, 0, 0, float(w), float(h));
        glEnable(GL_DEPTH_TEST);

        imgui.render();
    }
}