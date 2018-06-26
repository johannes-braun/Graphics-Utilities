layout(location=0) flat out int axis;

out gl_PerVertex
{
    vec4 gl_Position;
};

uniform mat4 view_projection;
uniform float scale = 4.f;

void main()
{
    axis = gl_VertexID / 2;
    int fac = gl_VertexID % 2;
    vec4 position = vec4(0, 0, 0, 1);
    position[axis] = scale*fac;
    gl_Position = view_projection * position;
}