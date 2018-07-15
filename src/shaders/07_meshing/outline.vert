#version 460 core

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

layout(binding = 0, std430) restrict readonly buffer HalfEdges
{
	half_edge half_edges[];
};

layout(binding = 1, std430) restrict readonly buffer Vertices
{
	vertex3d vertices[];
};

layout(binding = 0, std140) uniform Camera
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