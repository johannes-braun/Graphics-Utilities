layout(location=0) in vec3 position;
layout(location=1) in vec2 uv;
layout(location=2) in vec3 normal;

layout(location=0) out struct {
    vec3 position;
    vec2 uv;
    vec3 normal;
} vertex_out;

out gl_PerVertex
{
    vec4 gl_Position;
};

uniform mat4 view_projection;

void main()
{
    gl_Position = view_projection * vec4(position, 1);

    vertex_out.position = position;
    vertex_out.uv = uv;
    vertex_out.normal = normal;
}