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

    uint32_t next_index(const uint32_t& e)
    {
        const auto n = e + 1;
        return 3 * (e / 3) + n % 3;
    }
    uint32_t prev_index(const uint32_t& e)
    {
        const auto n = e - 1;
        return 3 * (e / 3) + (n + 3) % 3;
    }

    half_edge& next_of(const uint32_t& e) { return half_edges[next_index(e)]; }

    half_edge& prev_of(const uint32_t& e) { return half_edges[prev_index(e)]; }

    half_edge& first_edge(uint32_t face_index) { return half_edges[3 * face_index]; }

    std::vector<point>     vertices;
    std::vector<half_edge> half_edges;
};

int main()
{
    // --- Context ---
    gfx::context_options opt;
    opt.window_title        = "[07] Meshing";
    opt.debug               = true;
    opt.framebuffer_samples = 1;
    auto context            = gfx::context::create(opt);
    context->make_current();

    gfx::scene_file                                    scene("bunny.dae");
    half_edge_structure                                structure(scene.meshes[0].indices);
    gfx::device_buffer<half_edge_structure::half_edge> half_edge_buffer(gfx::buffer_usage::storage, structure.half_edges);
    gfx::vertex_input                                  mesh_input;
    mesh_input.set_assembly(gfx::topology::triangle_list);

    for (int i = 0; i < scene.meshes[0].vertices.size(); ++i) {
        const int he_start                    = structure.vertices.at(i).halfedge;
        int       he_a                        = he_start;
        scene.meshes[0].vertices.at(i).normal = {0, 0, 0};
        do
        {
            const auto&     vpos            = scene.meshes[0].vertices.at(i).position;
            const int       he_b            = structure.next_index(he_a);
            const glm::vec3 ab              = vpos - scene.meshes[0].vertices.at(structure.next_of(he_a).vertex).position;
            const glm::vec3 ca              = scene.meshes[0].vertices.at(structure.next_of(he_b).vertex).position - vpos;
            const int       he_a_op         = structure.half_edges.at(he_a).opposite;
            float           angle           = glm::acos(dot(ab, -ca) / (length(ab) * length(ca)));
            const glm::vec3 triangle_normal = normalize(cross(glm::vec3(ab), glm::vec3(-ca)));
            scene.meshes[0].vertices.at(i).normal += triangle_normal * angle;
            if (he_a_op == ~0u) {
                break;
            }
            he_a = structure.next_index(he_a_op);
        } while (he_a != he_start && he_a != ~0u);
    }
    gfx::device_buffer<gfx::vertex3d> mesh_vertices(gfx::buffer_usage::storage, scene.meshes[0].vertices);


    gl::pipeline mesh_sh;
    mesh_sh[GL_VERTEX_SHADER]   = std::make_shared<gl::shader>("07_meshing/mesh.vert");
    mesh_sh[GL_FRAGMENT_SHADER] = std::make_shared<gl::shader>("07_meshing/mesh.frag");

    struct camera_data
    {
        glm::mat4 view;
        glm::mat4 proj;
    };
    gfx::host_buffer<camera_data> camera_buffer(1);
    gfx::camera                   camera;
    gfx::camera_controller        controller;

    gfx::state_info mesh_state;
    mesh_state.rasterizer.cull                   = gfx::cull_mode::back;
    mesh_state.multisample.sample_shading_enable = true;
    mesh_state.multisample.samples               = gfx::sample_count::x8;

    gfx::device_image color_attachment(gfx::img_type::image2d, gfx::rgba16f, {1280, 720, 1}, gfx::sample_count::x8);
    gfx::device_image depth_attachment(gfx::img_type::image2d, gfx::d32f, {1280, 720, 1}, gfx::sample_count::x8);
    gfx::device_image resolve_attachment(gfx::img_type::image2d, gfx::rgba16f, {1280, 720, 1}, 1);

    gl::framebuffer render_fbo;
    glNamedFramebufferTexture(render_fbo, GL_COLOR_ATTACHMENT0, color_attachment, 0);
    glNamedFramebufferTexture(render_fbo, GL_DEPTH_ATTACHMENT, depth_attachment, 0);

    gl::framebuffer resolve_fbo;
    glNamedFramebufferTexture(resolve_fbo, GL_COLOR_ATTACHMENT0, resolve_attachment, 0);

    while (context->run()) {
        render_fbo.set_drawbuffer(GL_COLOR_ATTACHMENT0);
        render_fbo.bind();
        render_fbo.clear(0, {0.1f, 0.1f, 0.1f, 1.f});
        render_fbo.clear(0.f, 0);


        controller.update(camera);
        camera_buffer[0].view = glm::inverse(camera.transform_mode.matrix());
        camera_buffer[0].proj = camera.projection_mode.matrix();

        gfx::apply(mesh_state);
        mesh_sh.bind();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, half_edge_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mesh_vertices);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera_buffer);
        mesh_input.draw(structure.half_edges.size());


        // RESOLVE --------------------------------
        render_fbo.set_readbuffer(GL_COLOR_ATTACHMENT0);
        resolve_fbo.set_drawbuffer(GL_COLOR_ATTACHMENT0);
        render_fbo.blit(resolve_fbo, 0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        resolve_fbo.set_readbuffer(GL_COLOR_ATTACHMENT0);
        render_fbo.set_drawbuffer(GL_COLOR_ATTACHMENT0);

        resolve_fbo.blit(gl::framebuffer::zero(), 0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }
}

half_edge_structure::half_edge_structure(const std::vector<gfx::index32>& indices, uint32_t max_index)
{
    auto time_point = std::chrono::steady_clock::now();
    if (max_index == ~0u) max_index = *std::max_element(indices.begin(), indices.end()) + 1;
    std::vector</* base_vertex to */ std::vector<uint32_t> /* outgoing_halfedges */> vertex_to_halfedge(max_index);
    vertices.resize(max_index);
    half_edges.resize(indices.size());

    for (int i = 0; i < indices.size(); ++i) {
        half_edges[i].vertex          = indices[i];
        vertices[indices[i]].halfedge = i;
        vertex_to_halfedge[indices[i]].push_back(i);
    }

#pragma omp parallel for schedule(dynamic)
    for (int begin = 0; begin < half_edges.size(); ++begin) {
        for (uint32_t end : vertex_to_halfedge[indices[next_index(begin)]]) {
            if (half_edges[next_index(end)].vertex == half_edges[begin].vertex) {
                if (half_edges[begin].opposite == ~0u)
                    half_edges[begin].opposite = end;
                else
                    gfx::dlog << "Degenerate. This mesh is non-manifold.";
                break;
            }
        }
    }

    gfx::ilog << "Built in " << (std::chrono::steady_clock::now() - time_point).count() << "ns";
}