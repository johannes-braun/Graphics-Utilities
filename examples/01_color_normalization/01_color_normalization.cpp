#include <Eigen/Eigenvalues>
#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>
#include <numeric>
#include <opengl/opengl.hpp>

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
    Eigen::Matrix3d                            matrix_eigen = reinterpret_cast<const Eigen::Matrix3d&>(matrix);
    solver.compute(matrix_eigen);

    eigenresults    results;
    Eigen::Matrix3d vectors = solver.eigenvectors().real();
    Eigen::Vector3d values  = solver.eigenvalues().real();
    results.eigenvalues     = reinterpret_cast<const glm::dvec3&>(values);
    results.eigenvectors    = reinterpret_cast<const glm::dmat3&>(vectors);
    return results;
}

axis_transformation principal_axis_transformation(glm::u8vec3* pixels, const size_t count)
{
    const double     color_to_float      = 1.0 / 255.0;
    const glm::dvec3 main_axis_rgb       = glm::normalize(glm::dvec3(1.0));
    const glm::dvec3 main_axis_pomierski = glm::dvec3(0, 0, 1);
    const glm::dmat3 pomierski{0.5, -0.875, 1.5, -1.0, 0, 1.5, 0.5, 0.875, 1.5};
    const glm::dmat3 pomierski_inverse = inverse(pomierski);

    const glm::dvec3 average_rgb = std::accumulate(pixels,
                                                   pixels + count,
                                                   glm::dvec3(0.0),
                                                   [color_to_float](const glm::dvec3& lhs, const glm::u8vec3& rhs) {
                                                       return lhs + glm::dvec3(rhs) * color_to_float;
                                                   }) /
                                   static_cast<double>(count);
    const glm::dvec3 average_pomierski = pomierski * average_rgb;

    const glm::dmat4 translate_rgb            = glm::translate(-average_rgb);
    const glm::dmat4 translate_back_rgb       = glm::translate(main_axis_rgb * length(average_rgb));
    const glm::dmat4 translate_pomierski      = glm::translate(-average_pomierski);
    const glm::dmat4 translate_back_pomierski = glm::translate(main_axis_pomierski * length(average_pomierski));

    const glm::dmat3 covariance_rgb = std::accumulate(pixels,
                                                      pixels + count,
                                                      glm::dmat3(0.0),
                                                      [color_to_float, average_rgb](const glm::dmat3& lhs, const glm::u8vec3& rhs) {
                                                          const glm::dvec3 error = glm::dvec3(rhs) * color_to_float - average_rgb;
                                                          return lhs + outerProduct(error, error);
                                                      }) /
                                      static_cast<double>(count);
    const glm::dmat3 covariance_pomierski = pomierski * covariance_rgb * transpose(pomierski);

    const eigenresults eigen_rgb       = compute_eigen(covariance_rgb);
    const eigenresults eigen_pomierski = compute_eigen(covariance_pomierski);

    const auto get_axis_basic = [](const glm::dmat3& covariance) {
        double length  = glm::length(covariance[0]);
        int    current = 0;
        if(glm::length(covariance[1]) > length)
            current = (length = glm::length(covariance[1]), 1);
        if(glm::length(covariance[2]) > length)
            current = (length = glm::length(covariance[2]), 2);
        return covariance[current] / length;
    };

    const auto get_axis_eigen = [](const eigenresults& eigen) {
        double length  = eigen.eigenvalues[0];
        int    current = 0;
        if(eigen.eigenvalues[1] > length)
            current = (length = eigen.eigenvalues[1], 1);
        if(eigen.eigenvalues[2] > length)
            current = (length = eigen.eigenvalues[2], 2);
        return eigen.eigenvectors[current];
    };

    const glm::dvec3 axis_basic_rgb       = get_axis_basic(covariance_rgb);
    const glm::dvec3 axis_eigen_rgb       = get_axis_eigen(eigen_rgb);
    const glm::dvec3 axis_basic_pomierski = get_axis_basic(covariance_pomierski);
    const glm::dvec3 axis_eigen_pomierski = get_axis_eigen(eigen_pomierski);

    const auto make_rotation = [](const glm::dvec3& axis, const glm::dvec3& main_axis) {
        const double angle = dot(axis, main_axis);
        return toMat4(angleAxis(abs(angle), (angle > 0) ? cross(axis, main_axis) : cross(-axis, main_axis)));
    };

    axis_transformation result;
    result.basic_rgb       = translate_back_rgb * make_rotation(axis_basic_rgb, main_axis_rgb) * translate_rgb;
    result.eigen_rgb       = translate_back_rgb * make_rotation(axis_eigen_rgb, main_axis_rgb) * translate_rgb;
    result.basic_pomierski = glm::dmat4(pomierski_inverse) * translate_back_pomierski *
                             make_rotation(axis_basic_pomierski, main_axis_pomierski) * translate_pomierski * glm::dmat4(pomierski);
    result.eigen_pomierski = glm::dmat4(pomierski_inverse) * translate_back_pomierski *
                             make_rotation(axis_eigen_pomierski, main_axis_pomierski) * translate_pomierski * glm::dmat4(pomierski);

    return result;
}

int main()
{
    gfx::context_options options;
    options.window_title        = "[01] Color Normalization";
    options.window_width        = 1280;
    options.window_height       = 720;
    options.framebuffer_samples = 8;
    options.debug               = true;
    auto context                = gfx::context::create(options);
    context->make_current();

    gfx::imgui imgui;

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    const auto source_data = gfx::file::open_dialog("Open Image", "../", {"*.jpg", "*.png"}, "Image Files");
    if(!source_data)
        return 0;

    gfx::host_image   picture(gfx::rgb8unorm, gfx::image_file(*source_data, gfx::bits::b8, 3));
    gfx::device_image texture(2, gfx::rgb8unorm, picture.extents(), picture.max_levels());
    texture.level(0) << picture;
    texture.generate_mipmaps();
    gfx::image_view   texture_view(gfx::view_type::image_2d, gfx::rgb8unorm, texture, 0, picture.max_levels(), 0, 1);
    const gl::sampler sampler;

    glm::u8vec3*        begin = reinterpret_cast<glm::u8vec3*>(picture.storage().data());
    axis_transformation trafo = principal_axis_transformation(begin, picture.extents().count());
    glm::vec3           average =
            glm::vec3(std::accumulate(begin, begin + picture.extents().count(), glm::u8vec3(0))) / (picture.extents().count() * 255.f);

    gfx::host_image   grid_image(gfx::rgba8unorm, gfx::image_file("grid.jpg", gfx::bits::b8, 4));
    gfx::device_image grid(2, gfx::rgba8unorm, grid_image.extents(), grid_image.max_levels());
    grid.level(0) << grid_image;
    grid.generate_mipmaps();
    gfx::image_view grid_view(gfx::view_type::image_2d, gfx::rgba8unorm, grid, 0, grid_image.max_levels(), 0, 1);

    gl::pipeline points_pipeline;
    points_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("01_color_normalization/points.vert");
    points_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("01_color_normalization/points.frag");

    gl::pipeline center_pipeline;
    center_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("01_color_normalization/center_point.vert");
    center_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("01_color_normalization/center_point.frag");

    gl::pipeline gizmo_pipeline;
    gizmo_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("01_color_normalization/gizmo.vert");
    gizmo_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("01_color_normalization/gizmo.frag");

    gl::pipeline img_pipeline;
    img_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("postfx/screen.vert");
    img_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("01_color_normalization/image.frag");

    gl::pipeline cube_pipeline;
    cube_pipeline[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("01_color_normalization/cube.vert");
    cube_pipeline[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("01_color_normalization/cube.frag");

    namespace cube = gfx::cube_preset;
    gfx::device_buffer<gfx::vertex3d> vbo(gfx::buffer_usage::vertex, cube::vertices);
    gfx::device_buffer<gfx::index32>  ibo(gfx::buffer_usage::index, cube::indices);
    gfx::vertex_input vao;
    vao.add_attribute(0, gfx::format::rgb32f, offsetof(gfx::vertex3d, position));
    vao.add_attribute(0, gfx::format::rg32f, offsetof(gfx::vertex3d, uv));
    vao.set_binding_info(0, sizeof(gfx::vertex3d), gfx::input_rate::vertex);

    //gl::vertex_array empty_vao;
    gfx::vertex_input lines_vao;
    lines_vao.set_assembly(gfx::topology::line_list);
    gfx::vertex_input points_vao;
    points_vao.set_assembly(gfx::topology::point_list);
    gfx::vertex_input tris_vao;
    tris_vao.set_assembly(gfx::topology::triangle_list);

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glLineWidth(4.f);

    gfx::camera camera;
    camera.transform_mode.position = glm::vec3(0, 0, 5);
    gfx::camera_controller controller;

    while(context->run())
    {
        imgui.new_frame();

        gl::framebuffer::zero().clear(0, {0.15f, 0.15f, 0.15f, 1.f});
        gl::framebuffer::zero().clear(0.f, 0);

        const auto id = sampler.sample(texture_view);
        ImGui::Begin("Primary Axis Transformation");
        static bool hat_en = true;
        ImGui::Checkbox("Enable Transformation", &hat_en);

        glm::mat4 patmat = glm::mat4(1.f);
        if(hat_en)
        {
            static int mode = 0;
            ImGui::Combo("Mode", &mode, " Basic (RGB) \0 Eigen (RGB) \0 Basic (POM) \0 Eigen (POM) \0");

            switch(mode)
            {
            case 0:
                patmat = trafo.basic_rgb;
                break;
            case 1:
                patmat = trafo.eigen_rgb;
                break;
            case 2:
                patmat = trafo.basic_pomierski;
                break;
            case 3:
                patmat = trafo.eigen_pomierski;
                break;
            default:;
            }
        }

        static float scale = 1.f;
        ImGui::DragFloat("Preview Scale", &scale, 0.01f, 0.f, 100.f);

        if(ImGui::Button("Load", ImVec2(ImGui::GetContentRegionAvailWidth() * 0.5f, 0)))
        {
            if(const auto source_data = gfx::file::open_dialog("Open Image", "../", {"*.jpg", "*.png"}, "Image Files"))
            {
                picture = gfx::host_image(gfx::rgb8unorm, gfx::image_file(*source_data, gfx::bits::b8, 3));
                texture = gfx::device_image(2, gfx::rgba8unorm, picture.extents(), picture.max_levels());
                texture.level(0) << picture;
                texture.generate_mipmaps();
                texture_view = gfx::image_view(gfx::view_type::image_2d, gfx::rgb8unorm, texture, 0, picture.max_levels(), 0, 1);

                begin   = reinterpret_cast<glm::u8vec3*>(picture.storage().data());
                trafo   = principal_axis_transformation(begin, picture.extents().count());
                average = glm::vec3(std::accumulate(begin, begin + picture.extents().count(), glm::u8vec3(0))) /
                          (picture.extents().count() * 255.f);
            }
        }
        ImGui::SameLine();
        if(ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
        {
            glm::u8vec3*             data_cast = reinterpret_cast<glm::u8vec3*>(picture.storage().data());
            std::vector<glm::u8vec3> new_img(picture.extents().count());
            std::transform(data_cast, data_cast + picture.extents().count(), new_img.begin(), [&patmat](const glm::u8vec3& vec) {
                return clamp(glm::vec3(patmat * glm::vec4(glm::vec3(vec) / 255.f, 1)) * 255.f, glm::vec3(0), glm::vec3(255.f));
            });

            if(const auto dst = gfx::file::save_dialog("Save output", "../", {"*.png"}, "PNG"))
                gfx::image_file::save_png(*dst, picture.extents().width, picture.extents().height, 3, &new_img[0][0]);
        }
        ImGui::End();

        controller.update(camera);

        glm::mat4 vp = camera.projection_mode.matrix() * glm::inverse(camera.transform_mode.matrix());

        points_pipeline.bind();
        points_pipeline[GL_VERTEX_SHADER]->uniform<glm::mat4>("hat_mat")         = hat_en ? patmat : glm::mat4(1.0);
        points_pipeline[GL_VERTEX_SHADER]->uniform<uint64_t>("picture")          = id;
        points_pipeline[GL_VERTEX_SHADER]->uniform<glm::mat4>("view_projection") = vp;
        points_vao.draw(picture.extents().width * picture.extents().height);

        glDisable(GL_DEPTH_TEST);
        center_pipeline.bind();
        center_pipeline[GL_VERTEX_SHADER]->uniform<glm::mat4>("hat_mat")         = hat_en ? patmat : glm::mat4(1.0);
        center_pipeline[GL_VERTEX_SHADER]->uniform<glm::mat4>("view_projection") = vp;
        center_pipeline[GL_VERTEX_SHADER]->uniform<glm::vec3>("center")          = average;
        points_vao.draw(1);
        glEnable(GL_DEPTH_TEST);

        gizmo_pipeline.bind();
        gizmo_pipeline[GL_VERTEX_SHADER]->uniform<glm::mat4>("view_projection") = vp;
        lines_vao.draw(6);

        glFrontFace(GL_CW);

        cube_pipeline.bind();
        cube_pipeline[GL_VERTEX_SHADER]->uniform<glm::mat4>("view_projection") = vp;
        cube_pipeline[GL_FRAGMENT_SHADER]->uniform<uint64_t>("tex")            = sampler.sample(grid_view);
        cube_pipeline[GL_FRAGMENT_SHADER]->uniform<glm::vec4>("tint")          = glm::vec4(1, 1, 1, 1);
        vao.bind_vertex_buffer(0, vbo, 0);
        vao.bind_index_buffer(ibo, gfx::index_type::uint32);
        vao.draw_indexed(ibo.capacity());

        glFrontFace(GL_CCW);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        cube_pipeline[GL_FRAGMENT_SHADER]->uniform<glm::vec4>("tint") = glm::vec4(1, 1, 1, -1);
        vao.draw_indexed(ibo.capacity());
        glDisable(GL_BLEND);

        glDisable(GL_DEPTH_TEST);
        glViewportIndexedf(0, 0, 0, picture.extents().width * scale, picture.extents().height * scale);
        img_pipeline.bind();
        img_pipeline[GL_FRAGMENT_SHADER]->uniform<glm::mat4>("hat_mat") = hat_en ? patmat : glm::mat4(1.0);
        img_pipeline[GL_FRAGMENT_SHADER]->uniform<uint64_t>("tex")      = id;
        tris_vao.draw(3);

        int w, h;
        glfwGetFramebufferSize(context->window(), &w, &h);
        glViewportIndexedf(0, 0, 0, float(w), float(h));
        glEnable(GL_DEPTH_TEST);

        imgui.render();
    }
}