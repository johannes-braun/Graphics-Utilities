layout(location = 0) in vec3 position;

layout(binding = 0) uniform Camera
{
    mat4 view;
    mat4 projection;
    vec3 camera_position;
};

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec4 out_position;
layout(location = 1) flat out mat4 inv_vp;
void main()
{
    out_position     = view * vec4(position, 1);
    gl_Position      = projection * out_position;
    inv_vp           = inverse(projection * view);
}
