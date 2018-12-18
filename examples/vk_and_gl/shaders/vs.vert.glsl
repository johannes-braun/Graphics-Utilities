//? #version 460 core
//? #include "instance.glsl"

#ifndef LAYOUTS_DEFINED
#define layout_buffer_binding_models set = 1, binding = 0
#define layout_buffer_binding_vertex set = 1, binding = buffer_binding_vertex
#define layout_buffer_binding_element set = 1, binding = buffer_binding_element
#define layout_buffer_binding_bvh set = 1, binding = buffer_binding_bvh
#endif

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(set = 0, binding = 0) uniform Camera
{
	mat4 view;
	mat4 proj;
	vec3 position;
} camera;

layout(std430, layout_buffer_binding_models) restrict readonly buffer Models
{
	basic_instance models[];
};

layout(location = 0) out vec3 out_normal;
layout(location = 1) flat out int draw_index;
layout(location = 2) out vec3 out_position;

void main()
{
	draw_index = gl_DrawID;
	out_normal = vec3(inverse(transpose(models[gl_DrawID].transform)) * vec4(normal, 0)); 
	out_position = vec3(models[gl_DrawID].transform * vec4(position, 1));
	gl_Position = camera.proj * camera.view * vec4(out_position, 1);
}