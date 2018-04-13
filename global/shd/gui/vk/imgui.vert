layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoords;
layout(location = 2) in vec4 color;

layout(push_constant) uniform uPushConstant
{
    vec2 scale;
    vec2 translate;
};

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out struct
{
    vec4 color;
    vec2 texcoords;
} vertex_output;

void main()
{
    vertex_output.color = color;
    vertex_output.texcoords = texcoords;
    gl_Position = vec4(position*scale + translate, 0, 1);
}