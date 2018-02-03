layout(binding = 0) restrict readonly buffer PosBuffer
{
    vec4 positions[];
};

uniform mat4 view_projection;

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
};

void main()
{
    gl_Position = view_projection * (vec4(vec3(4), 1) * positions[gl_VertexID]);
    gl_PointSize = 10.f;
}