out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
};

uniform vec3 center;
uniform mat4 view_projection;
uniform float scale = 4.f;
uniform vec3 offset = vec3(0);
uniform vec3 factor = vec3(1);

void main()
{
    vec3 real_center = clamp(factor * center + offset, 0, 1);
    gl_Position = view_projection * vec4(scale * real_center, 1);
    gl_PointSize = 8.f;
}