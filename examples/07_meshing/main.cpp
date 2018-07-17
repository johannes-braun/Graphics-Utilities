#define GFX_EXPOSE_APIS
#include <gfx/gfx.hpp>
#include <gfx/graphics/opengl/state_info.hpp>

#include <opengl/opengl.hpp>

struct half_edge_structure
{
    constexpr static uint32_t invalid_edge = ~0u;

    struct point
    {
        uint32_t halfedge;
    };
    struct half_edge
    {
        uint32_t opposite = invalid_edge;
        uint32_t vertex;
    };

    half_edge_structure(const std::vector<gfx::index32>& indices, uint32_t max_index = ~0u);

    uint32_t   next_index(const uint32_t& e);
    uint32_t   prev_index(const uint32_t& e);
    half_edge& next_of(const uint32_t& e);
    half_edge& prev_of(const uint32_t& e);
    half_edge& first_edge(uint32_t face_index);

    std::vector<point>     vertices;
    std::vector<half_edge> half_edges;
};

struct surrounding
{
    surrounding(half_edge_structure& str, uint32_t index) : _str(&str), _index(index), _start_index(index) {}

    surrounding begin() { return *this; }
    surrounding end() { return surrounding{*_str, ~0u}; }
    surrounding operator++()
    {
        const int he_a_op = _str->half_edges.at(_index).opposite;
        if (he_a_op == ~0u)
        {
            _index = ~0u;
            return *this;
        }
        _index = _str->next_index(he_a_op);
        if (_index == _start_index || _index == ~0u)
        {
            _index = ~0u;
            return *this;
        }
        return *this;
    }

    bool     operator!=(const surrounding& other) const { return _index != other._index || _str != other._str; }
    uint32_t operator*() const { return _index; }

private:
    uint32_t             _start_index;
    uint32_t             _index;
    half_edge_structure* _str;
};

int main()
{
    // --- Context ---
    gfx::context_options opt;
    opt.window_title        = "[07] Meshing";
    opt.debug               = false;
    opt.framebuffer_samples = 1;
    auto context            = gfx::context::create(opt);
    context->make_current();
    gfx::imgui imgui;

    gfx::scene_file                             scene("bunny.dae");
    half_edge_structure                         structure(scene.meshes[0].indices);
    gfx::buffer<half_edge_structure::half_edge> half_edge_buffer(gfx::buffer_usage::storage, structure.half_edges);

    for (int i = 0; i < scene.meshes[0].vertices.size(); ++i)
    {
        const int he_start                    = structure.vertices.at(i).halfedge;
        int       he_a                        = he_start;
        scene.meshes[0].vertices.at(i).normal = {0, 0, 0};

        do
        {
            const auto& vpos = scene.meshes[0].vertices.at(i).position;
            const int   he_b = structure.next_index(he_a);

            const glm::vec3 ab = scene.meshes[0].vertices.at(structure.next_of(he_a).vertex).position - vpos;
            const glm::vec3 ac = scene.meshes[0].vertices.at(structure.next_of(he_b).vertex).position - vpos;

            const int       he_a_op         = structure.half_edges.at(he_a).opposite;
            float           angle           = glm::acos(dot(ab, ac) / (length(ab) * length(ac)));
            const glm::vec3 triangle_normal = cross(ab, ac);
            scene.meshes[0].vertices.at(i).normal += triangle_normal * angle;
            if (he_a_op == ~0u) break;
            he_a = structure.next_index(he_a_op);
        } while (he_a != he_start && he_a != ~0u);
        scene.meshes[0].vertices.at(i).normal = normalize(scene.meshes[0].vertices.at(i).normal);
    }

    gfx::buffer<gfx::vertex3d> mesh_vertices(gfx::buffer_usage::storage, scene.meshes[0].vertices);

    auto mesh_input = std::make_shared<gfx::vertex_input>();
    mesh_input->set_assembly(gfx::topology::triangle_list);
    auto point_input = std::make_shared<gfx::vertex_input>();
    point_input->set_assembly(gfx::topology::point_list);
    point_input->add_attribute(0, gfx::rgb32f, 0);
    point_input->set_binding_info(0, sizeof(glm::vec3), gfx::input_rate::vertex);

    auto mesh_state                               = std::make_shared<gfx::state_info>();
    mesh_state->rasterizer.cull                   = gfx::cull_mode::back;
    mesh_state->multisample.sample_shading_enable = true;
    mesh_state->multisample.samples               = gfx::sample_count::x8;
    mesh_state->rasterizer.polygon_mode           = gfx::poly_mode::fill;
    auto outline_state                            = std::make_shared<gfx::state_info>(*mesh_state);
    outline_state->rasterizer.cull                = gfx::cull_mode::front;
    auto points_state                             = std::make_shared<gfx::state_info>(*mesh_state);
    points_state->rasterizer.cull                 = gfx::cull_mode::none;

    gfx::graphics_pipeline mesh_pipeline(mesh_input, mesh_state);
    mesh_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::spirv, "07_meshing/mesh.frag.spv-opengl"));
    mesh_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::spirv, "07_meshing/mesh.vert.spv-opengl"));
    gfx::graphics_pipeline outline_pipeline(mesh_input, outline_state);
    outline_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::spirv, "07_meshing/outline.vert.spv-opengl"));
    outline_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::spirv, "07_meshing/outline.frag.spv-opengl"));
    gfx::graphics_pipeline points_pipeline(point_input, points_state);
    points_pipeline.attach(gfx::shader_type::vert, gfx::shader(gfx::shader_format::spirv, "07_meshing/points.vert.spv-opengl"));
    points_pipeline.attach(gfx::shader_type::frag, gfx::shader(gfx::shader_format::spirv, "07_meshing/points.frag.spv-opengl"));

    struct camera_data
    {
        glm::mat4 view;
        glm::mat4 proj;
    };
    gfx::hbuffer<camera_data> camera_buffer(1);
    gfx::camera               camera;
    gfx::camera_controller    controller;

    gfx::image color_attachment(gfx::img_type::image2d, gfx::rgba16f, {1280, 720, 1}, gfx::sample_count::x8);
    gfx::image depth_attachment(gfx::img_type::image2d, gfx::d32f, {1280, 720, 1}, gfx::sample_count::x8);
    gfx::image resolve_attachment(gfx::img_type::image2d, gfx::rgba16f, {1280, 720, 1}, 1);

    gl::framebuffer render_fbo;
    glNamedFramebufferTexture(render_fbo, GL_COLOR_ATTACHMENT0, color_attachment, 0);
    glNamedFramebufferTexture(render_fbo, GL_DEPTH_ATTACHMENT, depth_attachment, 0);

    gl::framebuffer resolve_fbo;
    glNamedFramebufferTexture(resolve_fbo, GL_COLOR_ATTACHMENT0, resolve_attachment, 0);

    gfx::hbuffer<glm::vec3> point_buffer(50000);
    gfx::buffer<glm::vec3>  point_buffer_device(gfx::buffer_usage::storage, 50000);
    gfx::keyboard_button    plus(GLFW_KEY_KP_ADD);
    gfx::keyboard_button    minus(GLFW_KEY_KP_SUBTRACT);
    std::vector<glm::vec3>  points;

    gfx::descriptor_set main_desc;
    main_desc.set(gfx::descriptor_type::storage_buffer, 0, half_edge_buffer);
    main_desc.set(gfx::descriptor_type::storage_buffer, 1, mesh_vertices);
    main_desc.set(gfx::descriptor_type::uniform_buffer, 0, camera_buffer);

    gfx::commands cmd;

    while (context->run())
    {
        imgui.new_frame();
        render_fbo.set_drawbuffer(GL_COLOR_ATTACHMENT0);
        render_fbo.bind();
        render_fbo.clear(0, {0.1f, 0.1f, 0.1f, 1.f});
        render_fbo.clear(0.f, 0);

        controller.update(camera);
        camera_buffer[0].view = glm::inverse(camera.transform_mode.matrix());
        camera_buffer[0].proj = camera.projection_mode.matrix();

        cmd.reset();
        gfx::clear_value clear[]{glm::vec4{0.1f, 0.1f, 0.1f, 1.f}, gfx::depth_stencil{0.f, 0}};
        cmd.begin_pass(clear, 2, mygl::framebuffer(render_fbo));
        cmd.bind_descriptors(&main_desc, 1);
        cmd.bind_pipeline(mesh_pipeline);
        cmd.draw(structure.half_edges.size());
        cmd.bind_pipeline(outline_pipeline);
        cmd.draw(structure.half_edges.size());

        ImGui::Begin("Settings");
        static int radius = 24;
        plus.update(context->window());
        minus.update(context->window());
        if (plus.state() == gfx::button_state::press || minus.state() == gfx::button_state::press)
        {
            if (plus.state() == gfx::button_state::press)
                ++radius;
            else
                --radius;

            std::unordered_set<uint32_t>  vertices{1};
            std::unordered_set<glm::vec3> surrounding_points;
            for (int i = 0; i < radius; ++i)
            {
                std::unordered_set<uint32_t> tmp{};
                for (const auto& padois : vertices)
                    for (const auto& s : surrounding(structure, padois))
                    {
                        const auto bla = scene.meshes[0].vertices.at(structure.next_of(s).vertex).position;
                        if (surrounding_points.count(bla) == 0)
                        {
                            surrounding_points.emplace(bla);
                            tmp.emplace(structure.next_index(s));
                        }
                    }
                vertices = {tmp.begin(), tmp.end()};
            }
            points = std::vector<glm::vec3>{surrounding_points.begin(), surrounding_points.end()};
            memcpy(point_buffer.data(), points.data(), points.size() * sizeof(glm::vec3));
        }

        ImGui::DragInt("Radius", &radius, 0.1f, 0, 1000);
        ImGui::End();

        point_buffer_device.fill_from(point_buffer, 0, 0, points.size());

        cmd.bind_pipeline(points_pipeline);
        cmd.bind_vertex_buffer(0, point_buffer_device);
        cmd.draw(points.size());
        cmd.execute();

        // RESOLVE --------------------------------
        render_fbo.set_readbuffer(GL_COLOR_ATTACHMENT0);
        resolve_fbo.set_drawbuffer(GL_COLOR_ATTACHMENT0);
        render_fbo.blit(resolve_fbo, 0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        resolve_fbo.set_readbuffer(GL_COLOR_ATTACHMENT0);
        render_fbo.set_drawbuffer(GL_COLOR_ATTACHMENT0);

        resolve_fbo.blit(gl::framebuffer::zero(), 0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        imgui.render();
    }
}

half_edge_structure::half_edge_structure(const std::vector<gfx::index32>& indices, uint32_t max_index)
{
    auto time_point = std::chrono::steady_clock::now();
    if (max_index == ~0u) max_index = *std::max_element(indices.begin(), indices.end()) + 1;
    std::vector</* base_vertex to */ std::vector<uint32_t> /* outgoing_halfedges */> vertex_to_halfedge(max_index);
    vertices.resize(max_index);
    half_edges.resize(indices.size());

    for (int i = 0; i < indices.size(); ++i)
    {
        half_edges[i].vertex          = indices[i];
        vertices[indices[i]].halfedge = i;
        vertex_to_halfedge[indices[i]].push_back(i);
    }

#pragma omp parallel for schedule(dynamic)
    for (int begin = 0; begin < half_edges.size(); ++begin)
    {
        for (uint32_t end : vertex_to_halfedge[indices[next_index(begin)]])
        {
            if (half_edges[next_index(end)].vertex == half_edges[begin].vertex)
            {
                half_edges[begin].opposite = end;
                break;
            }
        }
    }

    gfx::ilog << "Built in " << (std::chrono::steady_clock::now() - time_point).count() << "ns";
}


uint32_t half_edge_structure::next_index(const uint32_t& e)
{
    const auto n = e + 1;
    return 3 * (e / 3) + n % 3;
}
uint32_t half_edge_structure::prev_index(const uint32_t& e)
{
    const auto n = e - 1;
    return 3 * (e / 3) + (n + 3) % 3;
}
half_edge_structure::half_edge& half_edge_structure::next_of(const uint32_t& e)
{
    return half_edges[next_index(e)];
}
half_edge_structure::half_edge& half_edge_structure::prev_of(const uint32_t& e)
{
    return half_edges[prev_index(e)];
}
half_edge_structure::half_edge& half_edge_structure::first_edge(uint32_t face_index)
{
    return half_edges[3 * face_index];
}