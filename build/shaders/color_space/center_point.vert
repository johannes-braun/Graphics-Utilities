out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
};

uniform vec3 center;
uniform mat4 view_projection;
uniform mat4 hat_mat;
uniform float scale = 4.f;

void main()
{
    vec3 real_center = clamp(hat_mat * vec4(center, 1), 0, 1).xyz;
    gl_Position = view_projection * vec4(scale * real_center, 1);
    gl_PointSize = 8.f;
}