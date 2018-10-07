#include <executable.hpp>

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

void executable::init(gfx::context_options& options)
{
    options.window_title        = "[01] Color Normalization";
    options.window_width        = 1280;
    options.window_height       = 720;
    options.graphics_api        = gfx::gapi::vulkan;
    options.framebuffer_samples = gfx::sample_count::x8;
    options.debug               = true;
}

void executable::run()
{
    const auto source_data = gfx::file::open_dialog("Open Image", "../", {"*.jpg", "*.png"}, "Image Files");
    if (!source_data) return;

    gfx::himage        picture(gfx::rgba8unorm, *source_data);
    gfx::image         texture(picture);
    gfx::image_view    texture_view(gfx::imgv_type::image2d, texture);
    const gfx::sampler sampler;

    axis_transformation trafo   = principal_axis_transformation(picture);
    glm::vec3           average = [&] {
        glm::dvec4 color{0, 0, 0, 0};
        picture.each_pixel([&](const glm::uvec3& v) { color += picture.load(v); });
        return glm::dvec3(color / static_cast<double>(picture.extents().count()));
    }();

    gfx::image            grid(gfx::himage(gfx::rgba8unorm, "grid.jpg"));
    const gfx::image_view grid_view(gfx::imgv_type::image2d, grid);

    gfx::binding_layout main_layout;
    main_layout.push(gfx::binding_type::uniform_buffer).push(gfx::binding_type::sampled_image);

    gfx::pipe_state::binding_layouts bindings;
    bindings.layouts.push_back(&main_layout);

    gfx::pipe_state::rasterizer raster;
    raster.cull       = gfx::cull_mode::back;
    raster.line_width = 4.f;

    gfx::pipe_state::input_assembly ass;
    ass.primitive_topology = gfx::topology::point_list;

    gfx::pipe_state::depth_stencil dep;
    dep.depth_test_enable = true;

    gfx::pipe_state::multisample& msaa = msaa_state;

    gfx::pipe_state pipe_state;
    pipe_state.state_bindings       = &bindings;
    pipe_state.state_multisample    = &msaa;
    pipe_state.state_input_assembly = &ass;
    pipe_state.state_depth_stencil  = &dep;
    pipe_state.state_rasterizer     = &raster;

    auto shaders0 = {
        gfx::shader{gfx::shader_type::vert, "01_color_normalization/points.vert"},
        gfx::shader{gfx::shader_type::frag, "01_color_normalization/points.frag"},
    };
    gfx::graphics_pipeline points_pipe(pipe_state, pass_layout, shaders0);

    dep.depth_test_enable = false;
    auto shaders1         = {
        gfx::shader{gfx::shader_type::vert, "01_color_normalization/center_point.vert"},
        gfx::shader{gfx::shader_type::frag, "01_color_normalization/center_point.frag"},
    };
    gfx::graphics_pipeline center_pipe(pipe_state, pass_layout, shaders1);

    dep.depth_test_enable  = true;
    ass.primitive_topology = gfx::topology::line_list;
    auto shaders2          = {
        gfx::shader{gfx::shader_type::vert, "01_color_normalization/gizmo.vert"},
        gfx::shader{gfx::shader_type::frag, "01_color_normalization/gizmo.frag"},
    };
    gfx::graphics_pipeline gizmo_pipe(pipe_state, pass_layout, shaders2);

    dep.depth_test_enable  = false;
    ass.primitive_topology = gfx::topology::triangle_list;
    raster.cull            = gfx::cull_mode::none;
    auto shaders3          = {
        gfx::shader{gfx::shader_type::vert, "postfx/screen.vert"},
        gfx::shader{gfx::shader_type::frag, "01_color_normalization/image.frag"},
    };
    gfx::graphics_pipeline picture_pipe(pipe_state, pass_layout, shaders3);

    raster.cull           = gfx::cull_mode::back;
    dep.depth_test_enable = true;
    gfx::pipe_state::vertex_input input;
    input.attributes.emplace_back(0, 0, gfx::rgb32f, offsetof(gfx::vertex3d, position));
    input.attributes.emplace_back(1, 0, gfx::rg32f, offsetof(gfx::vertex3d, uv));
    input.bindings.emplace_back(0, sizeof(gfx::vertex3d));
    pipe_state.state_vertex_input = &input;

    raster.front_face = gfx::orientation::cw;
    auto shaders4     = {
        gfx::shader{gfx::shader_type::vert, "01_color_normalization/cube.vert"},
        gfx::shader{gfx::shader_type::frag, "01_color_normalization/cube.frag"},
    };
    gfx::graphics_pipeline cube_back_pipe(pipe_state, pass_layout, shaders4);

    raster.front_face = gfx::orientation::ccw;
    auto shaders5     = {
        gfx::shader{gfx::shader_type::vert, "01_color_normalization/cube.vert"},
        gfx::shader{gfx::shader_type::frag, "01_color_normalization/cube_front.frag"},
    };
    gfx::pipe_state::blending blend;
    pipe_state.state_blending                     = &blend;
    gfx::pipe_state::blend_attachment& attachment = blend.attachments.emplace_back();
    attachment.blendEnable                        = true;
    attachment.dstAlphaBlendFactor                = gfx::blend_factor::one_minus_src_alpha;
    attachment.srcAlphaBlendFactor                = gfx::blend_factor::src_alpha;
    attachment.alphaBlendOp                       = gfx::blend_op::op_add;
    attachment.dstColorBlendFactor                = gfx::blend_factor::one_minus_src_alpha;
    attachment.srcColorBlendFactor                = gfx::blend_factor::src_alpha;
    attachment.colorBlendOp                       = gfx::blend_op::op_add;
    gfx::graphics_pipeline cube_front_pipe(pipe_state, pass_layout, shaders5);

    gfx::buffer<gfx::vertex3d> vbo(gfx::buffer_usage::vertex, gfx::cube_preset::vertices);
    gfx::buffer<gfx::index32>  ibo(gfx::buffer_usage::index, gfx::cube_preset::indices);

    struct render_info
    {
        glm::mat4 vp;
        glm::mat4 transform_matrix;
        glm::vec3 average;
    };
    gfx::hbuffer<render_info> render_info_buffer(1);

    gfx::binding_set main_bindings(main_layout);
    main_bindings.bind(0, render_info_buffer);
    main_bindings.bind(1, texture_view, sampler);

    gfx::binding_set grid_bindings(main_layout);
    grid_bindings.bind(0, render_info_buffer);
    grid_bindings.bind(1, grid_view, sampler);

    while (frame())
    {
        ImGui::Begin("Settings");
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
                texture_view = gfx::image_view(gfx::imgv_type::image2d, texture.pixel_format(), texture, 0, texture.levels(), 0, 1);
                main_bindings.bind(1, texture_view, sampler);

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

        render_info_buffer[0].vp               = user_entity->get<gfx::camera_component>()->projection.matrix() * inverse(user_entity->get<gfx::transform_component>()->value.matrix());
        render_info_buffer[0].transform_matrix = hat_en ? patmat : glm::mat4(1.f);
        render_info_buffer[0].average          = average;

        gfx::viewport main_viewport(0, 0, float(w), float(h), 0.f, 1.f);

        current_command->begin_pass(*current_framebuffer);

        current_command->bind_pipeline(points_pipe, {&main_bindings});
        current_command->set_viewports(0, {&main_viewport, 1}, {});
        current_command->draw(picture.extents().width * picture.extents().height);

        current_command->bind_pipeline(center_pipe);
        current_command->set_viewports(0, {&main_viewport, 1}, {});
        current_command->draw(1);

        current_command->bind_pipeline(gizmo_pipe);
        current_command->set_viewports(0, {&main_viewport, 1}, {});
        current_command->draw(6);

        current_command->bind_pipeline(cube_back_pipe, {&grid_bindings});
        current_command->set_viewports(0, {&main_viewport, 1}, {});
        current_command->bind_vertex_buffer(vbo, 0);
        current_command->bind_index_buffer(ibo, gfx::index_type::uint32);
        current_command->draw_indexed(ibo.capacity());

        current_command->bind_pipeline(cube_front_pipe);
        current_command->set_viewports(0, {&main_viewport, 1}, {});
        current_command->bind_vertex_buffer(vbo, 0);
        current_command->bind_index_buffer(ibo, gfx::index_type::uint32);
        current_command->draw_indexed(ibo.capacity());

        constexpr float default_size = 256.f;
        const float     aspect       = static_cast<float>(picture.extents().height) / picture.extents().width;
        gfx::viewport pic_vp(0, main_viewport.height - (default_size * aspect * scale), default_size * scale, default_size * aspect * scale,
                             0.f, 1.f);
        current_command->bind_pipeline(picture_pipe, {&main_bindings});
        current_command->set_viewports(0, {&pic_vp, 1}, {});
        current_command->draw(3);

        current_command->end_pass();
    }
}