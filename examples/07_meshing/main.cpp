#include <executable.hpp>
#include <unordered_set>
#include "shaders/shaders.hpp"

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
        if (he_a_op == ~0u) {
            _index = ~0u;
            return *this;
        }
        _index = _str->next_index(he_a_op);
        if (_index == _start_index || _index == ~0u) {
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

void executable::init(gfx::context_options& opt)
{
    opt.window_title        = "[07] Meshing";
    opt.debug               = true;
    opt.framebuffer_samples = gfx::sample_count::x8;
    opt.graphics_api        = gfx::gapi::vulkan;
}

void executable::run()
{
    gfx::scene_file scene("bunny.dae");
    scene.mesh.collapse();
    half_edge_structure                         structure(scene.mesh.indices);
    gfx::buffer<half_edge_structure::half_edge> half_edge_buffer(gfx::buffer_usage::storage, structure.half_edges);

    for (int i = 0; i < scene.mesh.vertices.size(); ++i) {
        const int he_start               = structure.vertices.at(i).halfedge;
        int       he_a                   = he_start;
        scene.mesh.vertices.at(i).normal = {0, 0, 0};

        do
        {
            const auto& vpos = scene.mesh.vertices.at(i).position;
            const int   he_b = structure.next_index(he_a);

            const glm::vec3 ab = scene.mesh.vertices.at(structure.next_of(he_a).vertex).position - vpos;
            const glm::vec3 ac = scene.mesh.vertices.at(structure.next_of(he_b).vertex).position - vpos;

            const int       he_a_op         = structure.half_edges.at(he_a).opposite;
            float           angle           = glm::acos(dot(ab, ac) / (length(ab) * length(ac)));
            const glm::vec3 triangle_normal = cross(ab, ac);
            scene.mesh.vertices.at(i).normal += triangle_normal * angle;
            if (he_a_op == ~0u) break;
            he_a = structure.next_index(he_a_op);
        } while (he_a != he_start && he_a != ~0u);
        scene.mesh.vertices.at(i).normal = normalize(scene.mesh.vertices.at(i).normal);
    }
	gfx::buffer<gfx::vertex3d> mesh_vertices(gfx::buffer_usage::storage, scene.mesh.vertices);

	gfx::binding_layout bindings;
	bindings.push(gfx::binding_type::storage_buffer).push(gfx::binding_type::storage_buffer).push(gfx::binding_type::uniform_buffer);

	gfx::binding_set set(bindings);
	set.bind(0, half_edge_buffer);
	set.bind(1, mesh_vertices);
	set.bind(2, *camera_buffer);

	gfx::pipe_state::binding_layouts layouts;
	layouts.layouts.push_back(&bindings);

    gfx::pipe_state::input_assembly point_assembly;
    point_assembly.primitive_topology = gfx::topology::point_list;

    gfx::pipe_state::rasterizer rast;
    rast.cull = gfx::cull_mode::back;

    gfx::pipe_state state;
	state.state_bindings = &layouts;
    state.state_rasterizer  = &rast;
    state.state_multisample = &msaa_state;

    gfx::graphics_pipeline mesh_pipeline(state, pass_layout,
                                         {
                                             gfx::shader(gfx::shader_type::vert, gfx::spirv::meshing::shaders::mesh_vert),
                                             gfx::shader(gfx::shader_type::frag, gfx::spirv::meshing::shaders::mesh_frag),
                                         });

	rast.cull = gfx::cull_mode::front;
	gfx::graphics_pipeline outline_pipeline(state, pass_layout,
		{
			gfx::shader(gfx::shader_type::vert, gfx::spirv::meshing::shaders::outline_vert),
			gfx::shader(gfx::shader_type::frag, gfx::spirv::meshing::shaders::outline_frag),
		});

	state.state_input_assembly = &point_assembly;
	rast.cull = gfx::cull_mode::none;
	gfx::pipe_state::vertex_input input;
	input.attributes.emplace_back(0, 0, gfx::rgb32f, 0);
	input.bindings.emplace_back(0, sizeof(glm::vec3));
	state.state_vertex_input = &input;
	gfx::graphics_pipeline points_pipeline(state, pass_layout,
		{
			gfx::shader(gfx::shader_type::vert, gfx::spirv::meshing::shaders::points_vert),
			gfx::shader(gfx::shader_type::frag, gfx::spirv::meshing::shaders::points_frag),
		});

    gfx::hbuffer<glm::vec3> point_buffer(50000);
    gfx::buffer<glm::vec3>  point_buffer_device(gfx::buffer_usage::storage | gfx::buffer_usage::vertex, 50000);
    gfx::keyboard_button    plus(GLFW_KEY_KP_ADD);
    gfx::keyboard_button    minus(GLFW_KEY_KP_SUBTRACT);
    std::vector<glm::vec3>  points;
    
    while (frame()) 
	{
		current_command->begin_pass(*current_framebuffer);
		current_command->bind_pipeline(mesh_pipeline, { &set });
		current_command->draw(structure.half_edges.size());
		current_command->bind_pipeline(outline_pipeline);
        current_command->draw(structure.half_edges.size());

        ImGui::Begin("Settings");
        static int radius = 24;
        plus.update(context->window());
        minus.update(context->window());
        if (plus.state() == gfx::button_state::press || minus.state() == gfx::button_state::press) {
            if (plus.state() == gfx::button_state::press)
                ++radius;
            else
                --radius;

            std::unordered_set<uint32_t>  vertices{1};
            std::unordered_set<glm::vec3> surrounding_points;
            for (int i = 0; i < radius; ++i) {
                std::unordered_set<uint32_t> tmp{};
                for (const auto& padois : vertices)
                    for (const auto& s : surrounding(structure, padois)) {
                        const auto bla = scene.mesh.vertices.at(structure.next_of(s).vertex).position;
                        if (surrounding_points.count(bla) == 0) {
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

		gfx::buf_copy(point_buffer_device, point_buffer, points.size());

		current_command->bind_pipeline(points_pipeline);
        current_command->bind_vertex_buffer(point_buffer_device, 0);
		current_command->draw(points.size());
		current_command->end_pass();
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