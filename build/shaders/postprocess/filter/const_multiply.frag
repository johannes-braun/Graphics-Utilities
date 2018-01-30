
layout(location=0) in vec2 uv;

uniform sampler2D src_textures[];
#define this_texture (src_textures[0])
uniform float factor = 1.f;

layout(location=0) out vec4 color;

void main()
{
    color = texture(this_texture, uv) * factor;
}