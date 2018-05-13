#ifndef KERNEL
#define KERNEL_SIZE 15
#define KERNEL 0.105915,0.102673,0.093531,0.080066,0.064408,0.048689,0.034587,0.023089,
#endif

#define KERNEL_HALF (KERNEL_SIZE >> 1)

#ifndef DIRECTION
#define DIRECTION 1
#endif

layout(location=0) in vec2 uv;

layout(binding = 0) uniform sampler2D in_texture;

const float kernel[] = { KERNEL };

layout(location = 0) out vec4 color;

void main()
{
    const ivec2 tex_size = textureSize(in_texture, 0);//ivec2(1280, 720);
    const ivec2 pixel = ivec2(gl_FragCoord.xy);

    vec2 offset = vec2(0);
    color = textureLod(in_texture, (pixel + offset) / vec2(tex_size), 0) * kernel[0];
    for(int i=0; i<KERNEL_HALF; ++i)
    {
        offset[DIRECTION] = 2.0f*(i + 1);
        color += textureLod(in_texture, (pixel - offset) / vec2(tex_size), 0) * kernel[i + 1];
        color += textureLod(in_texture, (pixel + offset) / vec2(tex_size), 0) * kernel[i + 1];
    }
}