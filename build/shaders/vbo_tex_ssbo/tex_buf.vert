uniform mat4 view_projection;

uniform samplerBuffer picture;

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
};

void main()
{
    gl_Position = view_projection * (vec4(vec3(4), 1) * texelFetch(picture, gl_VertexID));
    gl_PointSize = 10.f;
}