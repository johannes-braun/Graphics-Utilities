layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 offset;
layout(location = 2) in vec4 color;

layout(binding = 0) uniform Data
{
    mat4 view_projection;
};

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out vec4 out_color;

void main()
{
    gl_Position = view_projection * vec4(pos + vec3(offset.x, 0, offset.y), 1);
    out_color = color;
}