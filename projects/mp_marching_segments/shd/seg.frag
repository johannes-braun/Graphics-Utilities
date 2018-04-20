layout(location = 0) in vec2 pixel;

layout(binding = 0) uniform Data
{
    mat4 vp;
    sampler2D tex;
};

layout(location = 0) out vec4 color;

void main()
{
    color = vec4(1, 0, 0, 1);// texelFetch(tex, ivec2(pixel), 0);
}