layout(location=0) in vec3 position;
layout(location=1) in vec2 uv;
layout(location=2) in vec3 normal;

layout(binding = 0) uniform Camera
{
    mat4 view;
    mat4 projection;
    vec3 camera_position;
};

layout(binding = 1) uniform ModelData
{
    mat4 model;
};

out gl_PerVertex 
{
    vec4 gl_Position;
};

layout(location=0) out vec3 out_position;
layout(location=1) out vec2 out_uv;
layout(location=2) out vec3 out_normal;

void main()
{
    out_position = (model*vec4(position, 1)).xyz;
    out_uv = uv;
    out_normal = normalize((inverse(transpose(model)) * vec4(normal, 1)).xyz);

    gl_Position = projection * view * model * vec4(position, 1);
}