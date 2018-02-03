uniform mat4 view_projection;

uniform sampler2D picture;

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
};

void main()
{
    ivec2 size = textureSize(picture, 0);
    gl_Position = view_projection * (vec4(vec3(4), 1) * texelFetch(picture, ivec2(gl_VertexID / size.y, gl_VertexID % size.y), 0));
    gl_PointSize = 10.f;
}