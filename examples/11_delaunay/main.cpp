#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>
#include <random>

int main()
{
    gfx::context_options opt;
    opt.window_title        = "[11] Delaunay";
    opt.debug               = true;
    opt.framebuffer_samples = 8;
    auto context            = gfx::context::create(opt);
    context->make_current();

    gfx::hbuffer<gfx::vertex3d> random_points;

    constexpr float                       extent = 5.f;
    std::mt19937                          gen(3891);
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    for (int i = 0; i < 20; ++i) random_points.emplace_back(glm::vec3(dist(gen), dist(gen), dist(gen)) * extent);

    gfx::buffer<gfx::vertex3d> random_points_vbo(gfx::buffer_usage::vertex, 20);
    random_points_vbo << random_points;

    auto state = std::make_shared<gfx::state_info>();
    // state->rasterizer.polygon_mode = gfx::poly_mode::line;

    auto input = std::make_shared<gfx::vertex_input>();
    input->set_assembly(gfx::topology::point_list);
    input->add_attribute(0, gfx::format::rgb32f, offsetof(gfx::vertex3d, position));
    input->set_binding_info(0, sizeof(gfx::vertex3d), gfx::input_rate::vertex);

    auto tri_input = std::make_shared<gfx::vertex_input>();
    tri_input->set_assembly(gfx::topology::triangle_list);
    tri_input->add_attribute(0, gfx::format::rgb32f, offsetof(gfx::vertex3d, position));
    tri_input->set_binding_info(0, sizeof(gfx::vertex3d), gfx::input_rate::vertex);

    gfx::graphics_pipeline points_pipeline(input, state);
    points_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "11_delaunay/points.vert"));
    points_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "11_delaunay/points.frag"));

    gfx::graphics_pipeline tri_pipeline(tri_input, state);
    tri_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::text, "11_delaunay/points.vert"));
    tri_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::text, "11_delaunay/points.frag"));

    gfx::camera                     camera;
    gfx::camera_controller          controller;
    gfx::hbuffer<gfx::camera::data> camera_buffer(1);

    std::vector<gfx::index32>  indices;
    std::vector<gfx::vertex3d> verts(random_points.begin(), random_points.end());
    const auto                 nearest = [&](const glm::vec3& pos) {
        return std::min_element(verts.begin(), verts.end(), [&](const gfx::vertex3d& a, const gfx::vertex3d& b) {
            return glm::distance2(pos, a.position) < glm::distance2(pos, b.position);
        });
    };

    // emplace a triangle
    auto v                         = verts.back();
    verts.back().metadata_position = 1;
    indices.emplace_back(static_cast<gfx::index32>(verts.size() - 1));
    auto next = std::min_element(verts.begin(), verts.end(), [&](const gfx::vertex3d& a, const gfx::vertex3d& b) {
        if (a.metadata_position == 1) return false;
        if (b.metadata_position == 1) return true;
        return glm::distance2(v.position, a.position) < glm::distance2(v.position, b.position);
    });
    indices.emplace_back(static_cast<gfx::index32>(std::distance(verts.begin(), next)));

    float max_angle_a = 0.f;
    float max_angle_b = 0.f;
    float min_dist_a  = 1000000000.f;
    float min_dist_b  = 1000000000.f;
    int   min         = -1;
    for (int i = 0; i < verts.size(); ++i) {
        if (verts[i].metadata_position == 1) continue;

        float md_a = distance2(verts[i].position, v.position);
        float md_b = distance2(verts[i].position, next->position);
        float an_a = dot(normalize(verts[i].position - v.position), normalize(next->position - v.position));
        float an_b = dot(normalize(verts[i].position - next->position), normalize(v.position - next->position));

        if ((an_a + an_b) * (1 / (md_a + md_b)) > (max_angle_a + max_angle_b) * (1 / (min_dist_a + min_dist_b))) {
            min         = i;
            max_angle_a = an_a;
            max_angle_b = an_b;
            min_dist_a  = md_a;
            min_dist_b  = md_b;
        }
    }
    verts[min].metadata_position = 1;
    indices.emplace_back(min);

    gfx::image img(gfx::host_image(gfx::rgba8unorm, "grid.jpg"));
    gfx::image_view img_view(gfx::imgv_type::image2d, img.pixel_format(), img, 0, img.levels(), 0, 1);
    gfx::sampler    smp;

    for (int t = 0, k = 0; t < 4 && k < 21; t += 3, k++) {
        // first triangle
        const auto& v0       = verts[indices[t]];
        const auto& v1       = verts[indices[t + 1]];
        const auto& v2       = verts[indices[t + 2]];
        const auto  centroid = (v0.position + v1.position + v2.position) / 3.f;

        float max_angle = 0.f;
        int   min       = -1;
        auto  norm      = normalize(cross(v2.position - v0.position, v1.position - v0.position));
        for (int i = 0; i < verts.size(); ++i) {
            if (verts[i].metadata_position == 1) continue;
            if (dot(norm, verts[i].position - centroid) < 0) continue;

            float md_a = distance2(verts[i].position, v0.position);
            float md_b = distance2(verts[i].position, v1.position);
            float md_c = distance2(verts[i].position, v2.position);


            float ang_a = dot(normalize(verts[i].position - v0.position), normalize(verts[i].position - v1.position));
            float ang_b = dot(normalize(verts[i].position - v1.position), normalize(verts[i].position - v2.position));
            float ang_c = dot(normalize(verts[i].position - v2.position), normalize(verts[i].position - v0.position));

            if (glm::compMin(glm::vec3(ang_a, ang_b, ang_c)) > max_angle) {
                min       = i;
                max_angle = glm::compMin(glm::vec3(ang_a, ang_b, ang_c));
            }
        }
        if (min == -1) continue;
        verts[min].metadata_position = 1;
        gfx::index32 ind             = min;
        indices.insert(std::end(indices),
                       {indices[t], indices[t + 1], ind, indices[t + 1], indices[t + 2], ind, indices[t + 2], indices[t + 0], ind});

        if (t != 0) {
            indices.erase(indices.begin() + t, indices.begin() + t + 3);
            t -= 3;
        }
    }

    gfx::buffer<gfx::index32> ibo(gfx::buffer_usage::index, indices);

    gfx::descriptor_set camera_set;
    camera_set.set(gfx::descriptor_type::uniform_buffer, 0, camera_buffer);
    camera_set.set(gfx::descriptor_type::sampled_texture, 0, img_view, smp);

    gfx::commands cmd;
    gfx::clear_value values[]{glm::vec4{0.2f, 0.2f, 0.2f, 1.f}, gfx::depth_stencil{0.f, 0u}};
    cmd.begin_pass(values, 2, mygl::framebuffer::zero);

    cmd.bind_pipeline(points_pipeline);
    cmd.bind_descriptors(&camera_set, 1);
    cmd.bind_vertex_buffer(0, random_points_vbo);
    cmd.draw(random_points.size());

    cmd.bind_pipeline(tri_pipeline);
    cmd.bind_descriptors(&camera_set, 1);
    cmd.bind_vertex_buffer(0, random_points_vbo);
    cmd.bind_index_buffer(ibo, gfx::index_type::uint32, 0);
    cmd.draw_indexed(ibo.capacity());

    while (context->run()) {
        controller.update(camera);
        camera_buffer[0] = camera.info();

        cmd.execute();
    }
}
