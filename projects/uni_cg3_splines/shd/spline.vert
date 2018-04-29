layout(location=0) in vec2 position;
layout(location=1) in vec4 color;

layout(binding=0) uniform Globals
{
    mat4 screen_matrix;
};

out gl_PerVertex { vec4 gl_Position; };
layout(location=0) out vec4 out_color;

void main()
{
    gl_Position = screen_matrix * vec4(position, 0.f, 1);
    out_color = color;
}