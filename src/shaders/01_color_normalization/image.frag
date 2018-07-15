#version 460 core

layout(location=0) in vec2 uv;

layout(binding = 0) uniform sampler2D tex;

layout(binding = 0) uniform render_info
{
    mat4 view_projection;
    mat4 hat_mat;
    vec3 average;
};

layout(location=0) out vec4 color;

void main()
{
    color = clamp(hat_mat * texture(tex, vec2(uv.x, 1-uv.y)), vec4(0), vec4(1));
}