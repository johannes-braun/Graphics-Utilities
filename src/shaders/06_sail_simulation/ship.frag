layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec4 color;

layout(binding = 1) uniform Model
{
    mat4 model_mat;
    mat4 normal_mat;
    vec4 mesh_color;
    sampler2D tex;
    bool has_texture;
};

void main()
{
    vec3 n = gl_FrontFacing ? normal : normal;

    color = (max(dot(n, normalize(vec3(1))), 0.f) + vec4(0.5f, 0.9f, 1.f, 1.f)) * mesh_color * (has_texture ? texture(tex, uv) : vec4(1));
}