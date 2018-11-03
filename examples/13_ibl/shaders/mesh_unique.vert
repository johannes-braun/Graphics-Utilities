#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

layout(binding = 0) uniform Camera
{
	mat4 view;
	mat4 proj;
} camera;

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out vec3 out_normal;

void main()
{
	vec4 m_position = /* model * */ vec4(position, 1);
	vec4 mvp_position = camera.proj * camera.view * m_position;
	
	out_position = m_position.xyz;
	out_uv = uv;
	out_normal = vec3(/*inverse(transpose(model)) * */vec4(normal, 0));

	gl_Position = mvp_position;
}