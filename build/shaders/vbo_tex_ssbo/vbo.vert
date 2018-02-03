layout(location = 0) in vec4 position;

uniform mat4 view_projection;

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
};

void main()
{
    gl_Position = view_projection * (vec4(vec3(4), 1) * position);
    gl_PointSize = 10.f;
}