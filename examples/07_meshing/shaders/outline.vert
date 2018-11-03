#version 460 core
#include <gfx.shaders/api.glsl> //! #include "../../../src/gfx.shaders/include/gfx.shaders/api.glsl"

const uint invalid_edge = ~0u;
struct point
{
    uint halfedge;
};
struct half_edge
{
    uint opposite;
    uint vertex;
};
struct vertex3d
{
    vec3 position;
    uint  metadata_position;
    vec3 normal;
    uint  metadata_normal;
    vec2 uv;
    ivec2  metadata_uv;
};

layout(loc_gl(0) loc_vk(0, 0), std430) restrict readonly buffer HalfEdges
{
	half_edge half_edges[];
};

layout(loc_gl(1) loc_vk(0, 1), std430) restrict readonly buffer Vertices
{
	vertex3d vertices[];
};

layout(loc_gl(0) loc_vk(0, 2), std140) uniform Camera
{
	mat4 view;
	mat4 proj;
} camera;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	const uint id = gl_VertexID;
	const vertex3d vertex = vertices[half_edges[id].vertex];

	gl_Position = camera.proj * camera.view * vec4(vertex.position + vertex.normal * 0.02f, 1);
}