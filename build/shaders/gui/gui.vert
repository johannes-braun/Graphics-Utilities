layout(location=0) in vec2 position;
layout(location=1) in vec2 uv;
layout(location=2) in vec4 color;

layout(location=0) out vec2 out_uv;
layout(location=1) out vec4 out_color;

uniform mat4 projection;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    out_uv = uv;
    out_color = color;
    
    gl_Position = projection * vec4(position.x, position.y, 1.f, 1);
}