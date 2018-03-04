// Inputs
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

// Outputs
out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out vec4 out_position;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out vec4 out_normal;

// Uniforms
uniform mat4 view_projection;
uniform mat4 model;

void main()
{
    const mat4 normal_matrix = inverse(transpose(model));

    gl_Position = view_projection * model * vec4(position, 1);
    out_position = model * vec4(position, 1);
    out_normal = normal_matrix * vec4(normal, 0);
    out_uv = uv;
}