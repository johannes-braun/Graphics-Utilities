layout(location = 0) flat in int color;

const vec3 colors[3] = vec3[](
    vec3(1, 0.3f, 0),
    vec3(0.3f, 1, 0),
    vec3(0, 0.3f, 1)
);

layout(location = 0) out vec4 out_color;
uniform int hovered = 1;

void main()
{
    out_color = (hovered == -2) ? vec4(vec3(1.f, 0.5f, 0), 0.2f) :  vec4(((hovered==color)?1.f:0.35f)*colors[color], 1);
}