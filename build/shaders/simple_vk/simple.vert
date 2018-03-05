layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoords;

layout(push_constant) uniform uPushConstant
{
    mat4 view_projection;
    mat4 inv_view;
};

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out struct
{
    vec4 position;
    vec2 texcoords;
    vec4 normal;
    vec4 cam_position;
} vertex_output;

void main()
{
    vertex_output.cam_position = inv_view[3];
    vertex_output.position = vec4(position, 1);
    vertex_output.texcoords = texcoords;
    vertex_output.normal = vec4(normal, 0);
    gl_Position = view_projection * vertex_output.position;
}