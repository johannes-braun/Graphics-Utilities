layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(binding = 0) uniform Data
{
    mat4 view_mat;
    mat4 projection_mat;
};

layout(binding = 1) uniform Model
{
    mat4 model_mat;
    mat4 normal_mat;
};

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;

void main()
{
    out_position = (model_mat * vec4(position, 1)).xyz;
    out_normal = (normal_mat * vec4(normal, 0)).xyz;
    out_uv = uv;

    gl_Position = projection_mat * view_mat * vec4(out_position, 1);
}