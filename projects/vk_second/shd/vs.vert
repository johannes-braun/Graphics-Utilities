#version 450 core
#extension GL_ARB_shader_draw_parameters: require

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

layout(set = 0, binding = 0) uniform Scene
{
    mat4 view;
    mat4 projection;
    vec3 camera_position;
    uint object_count;
};

out gl_PerVertex
{
    vec4 gl_Position;
};

struct mesh_info
{
    uint indexCount;
    uint instanceCount;
    uint firstIndex;
    int vertexOffset;
    uint firstInstance;
    mat4 model_matrix;
    vec4 bmin;
    vec4 bmax;
};
layout(set = 1, binding=0, std430) readonly buffer Models
{
    mesh_info infos[];
};

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out vec3 out_normal;

void main()
{
    out_position = (infos[gl_DrawIDARB].model_matrix * vec4(position, 1)).xyz;
    gl_Position = projection * view * infos[gl_DrawIDARB].model_matrix * vec4(position, 1);
    out_uv = uv;
    out_normal = normalize((inverse(transpose(infos[gl_DrawIDARB].model_matrix)) * vec4(normal, 0)).xyz);
}