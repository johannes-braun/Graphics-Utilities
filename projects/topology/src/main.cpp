#include <io/window.hpp>
#include <res/geometry.hpp>
#include <opengl/buffer.hpp>
#include <jpu/log.hpp>
#include <framework/gfx.hpp>

std::unique_ptr<io::window> main_window;

struct vertex
{
    uint32_t halfedge;
};

struct half_edge
{
    uint32_t opposite;
    uint32_t vertex;
    uint32_t face;
    uint32_t next;
    uint32_t prev;
};

struct face
{
    uint32_t halfedge;
};

std::vector<face> faces;
std::vector<vertex> vertices;
std::vector<half_edge> half_edges;
std::vector</* base_vertex to */ std::vector<uint32_t> /* outgoing_halfedges */> vertex_halfedge_association;

int main(int argc, const char** argv)
{
    gl::shader::set_include_directories(std::vector<gfx::files::path>{ "../shd", SOURCE_DIRECTORY "/global/shd" });
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    main_window = std::make_unique<io::window>(io::api::opengl, 1280, 720, "Topology");

    res::geometry_file geometry = res::load_geometry(gfx::file("bunny.dae"));
    std::vector<res::vertex>& mesh_vertices = geometry.meshes.get_by_index(0).vertices;
    std::vector<uint32_t>& mesh_indices = geometry.meshes.get_by_index(0).indices;

    faces.resize(mesh_indices.size() / 3);
    vertices.resize(mesh_vertices.size());
    half_edges.resize(mesh_indices.size());
    vertex_halfedge_association.resize(mesh_vertices.size());

    std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double, std::milli>> time_point = std::chrono::steady_clock::now();
    
#pragma omp parallel for
    for (int f = 0; f < faces.size(); ++f)
    {
        auto&& face = faces[f];
        face.halfedge = f * 3;
        half_edges[face.halfedge].face = f;
        half_edges[face.halfedge+1].face = f;
        half_edges[face.halfedge+2].face = f;

        half_edges[face.halfedge].next = face.halfedge + 1;
        half_edges[face.halfedge+1].next = face.halfedge + 2;
        half_edges[face.halfedge+2].next = face.halfedge;

        half_edges[face.halfedge].prev = face.halfedge + 2;
        half_edges[face.halfedge + 1].prev = face.halfedge;
        half_edges[face.halfedge + 2].prev = face.halfedge + 1;

        half_edges[face.halfedge].vertex = mesh_indices[face.halfedge];
        vertices[mesh_indices[face.halfedge]].halfedge = face.halfedge;

        half_edges[face.halfedge+1].vertex = mesh_indices[face.halfedge+1];
        vertices[mesh_indices[face.halfedge+1]].halfedge = face.halfedge+1;

        half_edges[face.halfedge+2].vertex = mesh_indices[face.halfedge+2];
        vertices[mesh_indices[face.halfedge+2]].halfedge = face.halfedge+2;

#pragma omp critical
        {
            vertex_halfedge_association[mesh_indices[face.halfedge]].push_back(face.halfedge);
            vertex_halfedge_association[mesh_indices[face.halfedge + 1]].push_back(face.halfedge + 1);
            vertex_halfedge_association[mesh_indices[face.halfedge + 2]].push_back(face.halfedge + 2);
        }
    }

#pragma omp parallel for schedule(dynamic)
    for (int h = 0; h < half_edges.size(); ++h)
    {
        half_edge& he_src = half_edges[h];
        half_edge& he_src_next = half_edges[he_src.next];
        for (uint32_t other_halfedge : vertex_halfedge_association.at(mesh_indices[he_src.next]))
        {
            half_edge& he_dst = half_edges[other_halfedge];
            half_edge& he_dst_next = half_edges[he_dst.next];
            
            if (he_dst_next.vertex == he_src.vertex)
            {
                he_src.opposite = other_halfedge;
                break;
            }
        }
    }

    log_i << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(std::chrono::steady_clock::now() - time_point).count();

    std::vector<uint32_t> opposites(half_edges.size());
    for (int i = 0; i < half_edges.size(); ++i)
    {
        int prev = opposites[half_edges[i].opposite]++;

        if (prev == 1)
            log_e << "Found degen: " << half_edges[i].opposite << " at position " << i;
    }

    return 0;
}