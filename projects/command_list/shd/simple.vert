#extension GL_NV_command_list : require
// Inputs
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

// Outputs
out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out vec4 out_position;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec2 out_uv;
layout(location = 3) out vec4 out_normal_world;

// Uniforms
layout(binding = 0, std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
    mat4 model;
};

void main()
{
    const mat4 normal_matrix = inverse(transpose(view * model));
    const mat4 normal_matrix_world = inverse(transpose(model));

    gl_Position = projection * view * model * vec4(position, 1);
    out_position = model * vec4(position, 1);
    out_normal = normal_matrix * vec4(normal, 0);
    out_normal_world = normal_matrix_world * vec4(normal, 0);
    out_uv = uv;
}
layout(commandBindableNV) uniform;