layout(location = 0) in vec3 position;

uniform mat4 model_view_projection;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) flat out int color;

void main()
{
    color = gl_VertexID / 24;
    gl_Position = model_view_projection * vec4(position, 1);
}