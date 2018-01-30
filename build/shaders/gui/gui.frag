layout(location=0) in vec2 uv;
layout(location=1) in vec4 color;

layout(location=0) out vec4 out_color;

uniform sampler2D img;

#define TEXTURE_RAW

void main()
{
#ifdef TEXTURE_RAW
    out_color = color * textureLod(img, uv, 0);
#else
    out_color = color * texture(img, uv);
#endif
}