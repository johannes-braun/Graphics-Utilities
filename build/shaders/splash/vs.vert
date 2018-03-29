layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;

layout(binding = 0, std140) uniform Data
{
    mat4 screen_mat;
    sampler2D img;
};

out gl_PerVertex{ vec4 gl_Position; };
layout(location = 0) out vec2 o_uv;

void main()
{
    gl_Position = (screen_mat * vec4(pos, 0, 1));
    o_uv = uv;
}