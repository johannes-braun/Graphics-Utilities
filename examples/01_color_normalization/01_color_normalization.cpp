#include <Eigen/Eigenvalues>
#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>
#include <gfx/math.hpp>
#include <numeric>
#include <opengl/opengl.hpp>

#include <gfx/graphics/opengl/state_info.hpp>

struct axis_transformation
{
    glm::mat4 basic_rgb;
    glm::mat4 eigen_rgb;

    glm::mat4 basic_pomierski;
    glm::mat4 eigen_pomierski;
};

axis_transformation principal_axis_transformation(glm::u8vec3* pixels, const size_t count)
{
    const double     color_to_float      = 1.0 / 255.0;
    const glm::dvec3 main_axis_rgb       = glm::normalize(glm::dvec3(1.0));
    const glm::dvec3 main_axis_pomierski = glm::dvec3(0, 0, 1);
    const glm::dmat3 pomierski{0.5, -0.875, 1.5, -1.0, 0, 1.5, 0.5, 0.875, 1.5};
    const glm::dmat3 pomierski_inverse = inverse(pomierski);

    const glm::dvec3 average_rgb =
        std::accumulate(pixels, pixels + count, glm::dvec3(0.0),
                        [color_to_float](const glm::dvec3& lhs, const glm::u8vec3& rhs) { return lhs + glm::dvec3(rhs) * color_to_float; })
        / static_cast<double>(count);
    const glm::dvec3 average_pomierski = pomierski * average_rgb;

    const glm::dmat4 translate_rgb            = glm::translate(-average_rgb);
    const glm::dmat4 translate_back_rgb       = glm::translate(main_axis_rgb * length(average_rgb));
    const glm::dmat4 translate_pomierski      = glm::translate(-average_pomierski);
    const glm::dmat4 translate_back_pomierski = glm::translate(main_axis_pomierski * length(average_pomierski));

    const glm::dmat3 covariance_rgb = std::accumulate(pixels, pixels + count, glm::dmat3(0.0),
                                                      [color_to_float, average_rgb](const glm::dmat3& lhs, const glm::u8vec3& rhs) {
                                                          const glm::dvec3 error = glm::dvec3(rhs) * color_to_float - average_rgb;
                                                          return lhs + outerProduct(error, error);
                                                      })
                                      / static_cast<double>(count);
    const glm::dmat3 covariance_pomierski = pomierski * covariance_rgb * transpose(pomierski);

    const auto eigen_rgb       = gfx::eig(covariance_rgb);
    const auto eigen_pomierski = gfx::eig(covariance_pomierski);

    const auto get_axis_basic = [](const glm::dmat3& covariance) {
        double length  = glm::length(covariance[0]);
        int    current = 0;
        if (glm::length(covariance[1]) > length) current = (length = glm::length(covariance[1]), 1);
        if (glm::length(covariance[2]) > length) current = (length = glm::length(covariance[2]), 2);
        return covariance[current] / length;
    };

    const auto get_axis_eigen = [](const auto& eigen) {
        double length  = eigen.eigenvalues[0];
        int    current = 0;
        if (eigen.eigenvalues[1] > length) current = (length = eigen.eigenvalues[1], 1);
        if (eigen.eigenvalues[2] > length) current = (length = eigen.eigenvalues[2], 2);
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
    result.basic_pomierski = glm::dmat4(pomierski_inverse) * translate_back_pomierski
                             * make_rotation(axis_basic_pomierski, main_axis_pomierski) * translate_pomierski * glm::dmat4(pomierski);
    result.eigen_pomierski = glm::dmat4(pomierski_inverse) * translate_back_pomierski
                             * make_rotation(axis_eigen_pomierski, main_axis_pomierski) * translate_pomierski * glm::dmat4(pomierski);

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

    const auto source_data = gfx::file::open_dialog("Open Image", "../", {"*.jpg", "*.png"}, "Image Files");
    if (!source_data) return 0;

    gfx::host_image    picture(gfx::rgb8unorm, *source_data);
    gfx::device_image  texture(picture);
    gfx::image_view    texture_view(gfx::imgv_type::image_2d, texture.pixel_format(), texture, 0, texture.levels(), 0, 1);
    const gfx::sampler sampler;

    glm::u8vec3*        begin = reinterpret_cast<glm::u8vec3*>(picture.storage().data());
    axis_transformation trafo = principal_axis_transformation(begin, picture.extents().count());
    glm::vec3           average =
        glm::vec3(std::accumulate(begin, begin + picture.extents().count(), glm::u8vec3(0))) / (picture.extents().count() * 255.f);

    gfx::device_image grid(gfx::host_image(gfx::rgba8unorm, "grid.jpg"));
    gfx::image_view   grid_view(gfx::imgv_type::image_2d, grid.pixel_format(), grid, 0, grid.levels(), 0, 1);

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

    gl::pipeline cube_front;
    cube_front[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("01_color_normalization/cube.vert");
    cube_front[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("01_color_normalization/cube_front.frag");

    gfx::device_buffer<gfx::vertex3d> vbo(gfx::buffer_usage::vertex, gfx::cube_preset::vertices);
    gfx::device_buffer<gfx::index32>  ibo(gfx::buffer_usage::index, gfx::cube_preset::indices);
    gfx::vertex_input                 vao;
    vao.add_attribute(0, gfx::format::rgb32f, offsetof(gfx::vertex3d, position));
    vao.add_attribute(0, gfx::format::rg32f, offsetof(gfx::vertex3d, uv));
    vao.set_binding_info(0, sizeof(gfx::vertex3d), gfx::input_rate::vertex);

    gfx::vertex_input lines_vao;
    lines_vao.set_assembly(gfx::topology::line_list);
    gfx::vertex_input points_vao;
    points_vao.set_assembly(gfx::topology::point_list);
    gfx::vertex_input tris_vao;
    tris_vao.set_assembly(gfx::topology::triangle_list);

    gfx::state_info default_state;
    default_state.depth_stencil.depth_test_enable   = true;
    default_state.rasterizer.cull                   = gfx::cull_mode::back;
    default_state.rasterizer.line_width             = 4.f;
    default_state.multisample.sample_shading_enable = true;
    default_state.multisample.samples               = gfx::sample_count::x8;

    gfx::state_info points_state                 = default_state;
    points_state.depth_stencil.depth_test_enable = false;

    gfx::state_info cube_backfaces       = default_state;
    cube_backfaces.rasterizer.front_face = gfx::orientation::cw;

    gfx::state_info cube_frontfaces                 = cube_backfaces;
    cube_frontfaces.rasterizer.front_face           = gfx::orientation::ccw;
    cube_frontfaces.depth_stencil.depth_test_enable = false;
    gfx::state_info::blend_attachment& color_att    = cube_frontfaces.blend.attachments.emplace_back();
    color_att.blendEnable                           = true;
    color_att.dstAlphaBlendFactor                   = gfx::blend_factor::one_minus_src_alpha;
    color_att.srcAlphaBlendFactor                   = gfx::blend_factor::src_alpha;
    color_att.alphaBlendOp                          = gfx::blend_op::op_add;
    color_att.dstColorBlendFactor                   = gfx::blend_factor::one_minus_src_alpha;
    color_att.srcColorBlendFactor                   = gfx::blend_factor::src_alpha;
    color_att.colorBlendOp                          = gfx::blend_op::op_add;

    gfx::state_info picture_state                 = default_state;
    picture_state.depth_stencil.depth_test_enable = false;

    gfx::camera camera;
    camera.transform_mode.position = glm::vec3(0, 0, 5);
    gfx::camera_controller controller;

    struct render_info
    {
        glm::mat4 vp;
        glm::mat4 transform_matrix;
        glm::vec3 average;
    };
    gfx::host_buffer<render_info> render_info_buffer(1);

    class command_buffer
    {
        public:
        void set_viewports(uint32_t first, const std::vector<gfx::viewport>& vps)
        {
            //_commands.push_back([ first, count, vps ]
            //{
            for (int i = first; i < first + vps.size(); ++i) {
                glViewportIndexedf(i, vps[i].x, vps[i].y, vps[i].width, vps[i].height);
                glDepthRangeIndexed(i, vps[i].min_depth, vps[i].max_depth);
            }
            //});
        }

        void run(std::function<void()> fun) { _commands.push_back(fun); }

        void commit()
        {
            for (const auto& x : std::exchange(_commands, {})) {
                x();
            }
        }

        private:
        std::vector<std::function<void()>> _commands;
    };

    command_buffer main_buffer;

    while (context->run()) {
        imgui.new_frame();

        gl::framebuffer::zero().clear(0, {0.1f, 0.1f, 0.1f, 1.f});
        gl::framebuffer::zero().clear(0.f, 0);

        ImGui::Begin("Primary Axis Transformation");
        static bool hat_en = true;
        ImGui::Checkbox("Enable Transformation", &hat_en);

        glm::mat4 patmat = glm::mat4(1.f);
        if (hat_en) {
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

        if (ImGui::Button("Load", ImVec2(ImGui::GetContentRegionAvailWidth() * 0.5f, 0))) {
            if (const auto source_data = gfx::file::open_dialog("Open Image", "../", {"*.jpg", "*.png"}, "Image Files")) {
                picture      = gfx::host_image(gfx::rgb8unorm, *source_data);
                texture      = gfx::device_image(picture);
                texture_view = gfx::image_view(gfx::imgv_type::image_2d, texture.pixel_format(), texture, 0, texture.levels(), 0, 1);

                begin   = reinterpret_cast<glm::u8vec3*>(picture.storage().data());
                trafo   = principal_axis_transformation(begin, picture.extents().count());
                average = glm::vec3(std::accumulate(begin, begin + picture.extents().count(), glm::u8vec3(0)))
                          / (picture.extents().count() * 255.f);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvailWidth(), 0))) {
            glm::u8vec3*             data_cast = reinterpret_cast<glm::u8vec3*>(picture.storage().data());
            std::vector<glm::u8vec3> new_img(picture.extents().count());
            std::transform(data_cast, data_cast + picture.extents().count(), new_img.begin(), [&patmat](const glm::u8vec3& vec) {
                return clamp(glm::vec3(patmat * glm::vec4(glm::vec3(vec) / 255.f, 1)) * 255.f, glm::vec3(0), glm::vec3(255.f));
            });

            if (const auto dst = gfx::file::save_dialog("Save output", "../", {"*.png"}, "PNG"))
                gfx::image_file::save_png(*dst, picture.extents().width, picture.extents().height, 3, &new_img[0][0]);
        }
        ImGui::End();

        int w, h;
        glfwGetFramebufferSize(context->window(), &w, &h);
        controller.update(camera);

        render_info_buffer[0].vp               = camera.projection_mode.matrix() * glm::inverse(camera.transform_mode.matrix());
        render_info_buffer[0].transform_matrix = hat_en ? patmat : glm::mat4(1.f);
        render_info_buffer[0].average          = average;

        // set 0 (layout 0)
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, render_info_buffer);

        // set 1 (layout 1)
        glBindSampler(0, sampler);
        glBindTextureUnit(0, texture_view);

        // set 2 (layout 1)
        // glBindSampler(0, sampler);
        // glBindTextureUnit(0, grid_view);

        gfx::viewport main_viewport(0, 0, float(w), float(h), 0.f, 1.f);

        gfx::apply(default_state);
        main_buffer.set_viewports(0, {main_viewport});
        points_pipeline.bind();
        glBindTextureUnit(0, texture_view);
        points_vao.draw(picture.extents().width * picture.extents().height);

        gfx::apply(points_state);
        main_buffer.set_viewports(0, {main_viewport});
        center_pipeline.bind();
        points_vao.draw(1);

        gfx::apply(default_state);
        main_buffer.set_viewports(0, {main_viewport});
        gizmo_pipeline.bind();
        lines_vao.draw(6);

        glBindTextureUnit(0, grid_view);

        gfx::apply(cube_backfaces);
        main_buffer.set_viewports(0, {main_viewport});
        cube_pipeline.bind();
        vao.bind_vertex_buffer(0, vbo, 0);
        vao.bind_index_buffer(ibo, gfx::index_type::uint32);
        vao.draw_indexed(ibo.capacity());

        gfx::apply(cube_frontfaces);
        main_buffer.set_viewports(0, {main_viewport});
        cube_front.bind();
        vao.draw_indexed(ibo.capacity());

        glBindTextureUnit(0, texture_view);

        gfx::apply(picture_state);
        gfx::viewport pic_vp(0, 0, picture.extents().width * scale, picture.extents().height * scale, 0.f, 1.f);
        main_buffer.set_viewports(0, {pic_vp});
        img_pipeline.bind();
        tris_vao.draw(3);

        imgui.render();
    }
}