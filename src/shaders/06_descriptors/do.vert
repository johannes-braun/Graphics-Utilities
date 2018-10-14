#version 460 core
//! #extension GL_KHR_vulkan_glsl : require

out gl_PerVertex
{
	vec4 gl_Position;
};

vec2 pts[3] = {
	vec2(0, 0.5f),
	vec2(-0.5f, -0.5f),
	vec2(0.5f, -0.5f)
};

layout(set = 0, binding = 0) uniform Matrix
{
	mat4 my_mat;
};

layout(location = 0) out vec3 albedo;

void main()
{
	albedo = vec3(0);
	albedo[gl_VertexIndex % 3] = 1.f;
	gl_Position = my_mat * vec4(pts[gl_VertexIndex], 0, 1);
}