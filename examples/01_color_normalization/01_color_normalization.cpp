#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>
#include <gfx/math.hpp>

#include <runnable.hpp>

struct axis_transformation
{
    glm::mat4 basic_rgb;
    glm::mat4 eigen_rgb;

    glm::mat4 basic_pomierski;
    glm::mat4 eigen_pomierski;
};

axis_transformation principal_axis_transformation(const gfx::himage& image)
{
    const glm::dvec3 main_axis_rgb       = normalize(glm::dvec3(1.0));
    const glm::dvec3 main_axis_pomierski = glm::dvec3(0, 0, 1);
    const glm::dmat3 pomierski{0.5, -0.875, 1.5, -1.0, 0, 1.5, 0.5, 0.875, 1.5};
    const glm::dmat3 pomierski_inverse = inverse(pomierski);

    const glm::dvec3 average_rgb = [&] {
        glm::dvec4 color{0, 0, 0, 0};
        image.each_pixel([&](const glm::uvec3& v) { color += image.load(v); });
        return glm::dvec3(color / static_cast<double>(image.extents().count()));
    }();

    const glm::dvec3 average_pomierski = pomierski * average_rgb;

    const glm::dmat4 translate_rgb            = translate(-average_rgb);
    const glm::dmat4 translate_back_rgb       = translate(main_axis_rgb * length(average_rgb));
    const glm::dmat4 translate_pomierski      = translate(-average_pomierski);
    const glm::dmat4 translate_back_pomierski = translate(main_axis_pomierski * length(average_pomierski));

    const glm::dmat3 covariance_rgb = [&] {
        glm::dmat3 matrix(0.0);
        for (int i = 0; i < image.extents().count(); ++i)
        {
            const glm::dvec3 error = glm::dvec3(image.load(image.extents().subpixel(i))) - average_rgb;
            matrix += outerProduct(error, error);
        }
        return matrix / static_cast<double>(image.extents().count());
    }();

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

    axis_transformation result{};
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
	runnable r;
	r.run();
}

void runnable::init(gfx::context_options& options)
{
	options.window_title        = "[01] Color Normalization";
	options.window_width        = 1280;
	options.window_height       = 720;
	options.framebuffer_samples = 8;
	options.debug               = true;
}

void runnable::run()
{
    const auto source_data = gfx::file::open_dialog("Open Image", "../", {"*.jpg", "*.png"}, "Image Files");
    if (!source_data) return;

    gfx::himage        picture(gfx::rgb8unorm, *source_data);
    gfx::image         texture(picture);
    gfx::image_view    texture_view(gfx::imgv_type::image_2d, texture.pixel_format(), texture, 0, texture.levels(), 0, 1);
    const gfx::sampler sampler;

    axis_transformation trafo   = principal_axis_transformation(picture);
    glm::vec3           average = [&] {
        glm::dvec4 color{0, 0, 0, 0};
        picture.each_pixel([&](const glm::uvec3& v) { color += picture.load(v); });
        return glm::dvec3(color / static_cast<double>(picture.extents().count()));
    }();

    gfx::image            grid(gfx::himage(gfx::rgba8unorm, "grid.jpg"));
    const gfx::image_view grid_view(gfx::imgv_type::image_2d, grid.pixel_format(), grid, 0, grid.levels(), 0, 1);

    auto default_state                               = std::make_shared<gfx::state_info>();
    default_state->depth_stencil.depth_test_enable   = true;
    default_state->rasterizer.cull                   = gfx::cull_mode::back;
    default_state->rasterizer.line_width             = 4.f;
    default_state->multisample.sample_shading_enable = true;
    default_state->multisample.samples               = gfx::sample_count::x8;

    const auto points_state                       = std::make_shared<gfx::state_info>(*default_state);
    points_state->depth_stencil.depth_test_enable = false;

    const auto picture_state                       = std::make_shared<gfx::state_info>(*default_state);
    picture_state->depth_stencil.depth_test_enable = false;

    auto cube_backfaces                   = std::make_shared<gfx::state_info>(*default_state);
    cube_backfaces->rasterizer.front_face = gfx::orientation::cw;

    auto cube_frontfaces                             = std::make_shared<gfx::state_info>(*cube_backfaces);
    cube_frontfaces->rasterizer.front_face           = gfx::orientation::ccw;
    cube_frontfaces->depth_stencil.depth_test_enable = false;
    gfx::state_info::blend_attachment& color_att     = cube_frontfaces->blend.attachments.emplace_back();
    color_att.blendEnable                            = true;
    color_att.dstAlphaBlendFactor                    = gfx::blend_factor::one_minus_src_alpha;
    color_att.srcAlphaBlendFactor                    = gfx::blend_factor::src_alpha;
    color_att.alphaBlendOp                           = gfx::blend_op::op_add;
    color_att.dstColorBlendFactor                    = gfx::blend_factor::one_minus_src_alpha;
    color_att.srcColorBlendFactor                    = gfx::blend_factor::src_alpha;
    color_att.colorBlendOp                           = gfx::blend_op::op_add;

    auto points_vao = std::make_shared<gfx::vertex_input>();
    points_vao->set_assembly(gfx::topology::point_list);
    auto lines_vao = std::make_shared<gfx::vertex_input>();
    lines_vao->set_assembly(gfx::topology::line_list);
    auto tris_vao = std::make_shared<gfx::vertex_input>();
    tris_vao->set_assembly(gfx::topology::triangle_list);
    auto cube_vao = std::make_shared<gfx::vertex_input>();
    cube_vao->set_assembly(gfx::topology::triangle_list);
    cube_vao->add_attribute(0, gfx::format::rgb32f, offsetof(gfx::vertex3d, position));
    cube_vao->add_attribute(0, gfx::format::rg32f, offsetof(gfx::vertex3d, uv));
    cube_vao->set_binding_info(0, sizeof(gfx::vertex3d), gfx::input_rate::vertex);

    gfx::graphics_pipeline point_pipeline(points_vao, default_state);
    point_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::spirv, "01_color_normalization/points.vert.spv-opengl"));
    point_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::spirv, "01_color_normalization/points.frag.spv-opengl"));

    gfx::graphics_pipeline center_pipeline(points_vao, points_state);
    center_pipeline.attach(gfx::shader_type::vert,
                           gfx::shader(gfx::shader_format::spirv, "01_color_normalization/center_point.vert.spv-opengl"));
    center_pipeline.attach(gfx::shader_type::frag,
                           gfx::shader(gfx::shader_format::spirv, "01_color_normalization/center_point.frag.spv-opengl"));

    gfx::graphics_pipeline gizmo_pipeline(lines_vao, default_state);
    gizmo_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::spirv, "01_color_normalization/gizmo.vert.spv-opengl"));
    gizmo_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::spirv, "01_color_normalization/gizmo.frag.spv-opengl"));

    gfx::graphics_pipeline img_pipeline(tris_vao, picture_state);
    img_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "postfx/screen.vert"));
    img_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::spirv, "01_color_normalization/image.frag.spv-opengl"));

    gfx::graphics_pipeline cube_pipeline(cube_vao, cube_backfaces);
    cube_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::spirv, "01_color_normalization/cube.vert.spv-opengl"));
    cube_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::spirv, "01_color_normalization/cube.frag.spv-opengl"));

    gfx::graphics_pipeline cube_front(cube_vao, cube_frontfaces);
    cube_front.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::spirv, "01_color_normalization/cube.vert.spv-opengl"));
    cube_front.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::spirv, "01_color_normalization/cube_front.frag.spv-opengl"));

    gfx::buffer<gfx::vertex3d> vbo(gfx::buffer_usage::vertex, gfx::cube_preset::vertices);
    gfx::buffer<gfx::index32>  ibo(gfx::buffer_usage::index, gfx::cube_preset::indices);

    struct render_info
    {
        glm::mat4 vp;
        glm::mat4 transform_matrix;
        glm::vec3 average;
    };
    gfx::hbuffer<render_info> render_info_buffer(1);

    gfx::descriptor_set main_desc;
    main_desc.set(gfx::descriptor_type::uniform_buffer, 0, render_info_buffer);
    main_desc.set(gfx::descriptor_type::sampled_texture, 0, texture_view, sampler);

    gfx::descriptor_set grid_desc;
    grid_desc.set(gfx::descriptor_type::uniform_buffer, 0, render_info_buffer);
    grid_desc.set(gfx::descriptor_type::sampled_texture, 0, grid_view, sampler);

    gfx::commands cmd;

    while (frame())
    {
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
            if (const auto new_data = gfx::file::open_dialog("Open Image", "../", {"*.jpg", "*.png"}, "Image Files"))
            {
                picture      = gfx::himage(gfx::rgb8unorm, *new_data);
                texture      = gfx::image(picture);
                texture_view = gfx::image_view(gfx::imgv_type::image_2d, texture.pixel_format(), texture, 0, texture.levels(), 0, 1);
                main_desc.set(gfx::descriptor_type::sampled_texture, 0, texture_view, sampler);

                trafo   = principal_axis_transformation(picture);
                average = [&] {
                    glm::dvec4 color{0, 0, 0, 0};
                    picture.each_pixel([&](const glm::uvec3& v) { color += picture.load(v); });
                    return glm::dvec3(color / static_cast<double>(picture.extents().count()));
                }();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
        {
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

        render_info_buffer[0].vp               = camera.projection_mode.matrix() * inverse(camera.transform_mode.matrix());
        render_info_buffer[0].transform_matrix = hat_en ? patmat : glm::mat4(1.f);
        render_info_buffer[0].average          = average;

        gfx::viewport main_viewport(0, 0, float(w), float(h), 0.f, 1.f);

        cmd.reset();
        gfx::clear_value values[]{glm::vec4{0.2f, 0.2f, 0.2f, 1.f}, gfx::depth_stencil{0.f, 0u}};
        cmd.begin_pass(*main_framebuffer);

        cmd.bind_descriptors(&main_desc, 1);

        cmd.bind_pipeline(point_pipeline);
        cmd.set_viewports(&main_viewport, 1, 0);
        cmd.draw(picture.extents().width * picture.extents().height);

        cmd.bind_pipeline(center_pipeline);
        cmd.set_viewports(&main_viewport, 1, 0);
        cmd.draw(1);

        cmd.bind_pipeline(gizmo_pipeline);
        cmd.set_viewports(&main_viewport, 1, 0);
        cmd.draw(6);

        cmd.bind_descriptors(&grid_desc, 1);

        cmd.bind_pipeline(cube_pipeline);
        cmd.bind_vertex_buffer(0, vbo);
        cmd.bind_index_buffer(ibo, gfx::index_type::uint32);
        cmd.draw_indexed(ibo.capacity());

        cmd.bind_pipeline(cube_front);
        cmd.draw_indexed(ibo.capacity());

        cmd.bind_descriptors(&main_desc, 1);

        gfx::viewport pic_vp(0, 0, picture.extents().width * scale, picture.extents().height * scale, 0.f, 1.f);
        cmd.bind_pipeline(img_pipeline);
        cmd.set_viewports(&pic_vp, 1, 0);
        cmd.draw(3);
		cmd.end_pass();

        cmd.execute();
    }
}