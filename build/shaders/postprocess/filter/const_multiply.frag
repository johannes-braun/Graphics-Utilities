
layout(location=0) in vec2 uv;

uniform sampler2D src_textures[];
#define this_texture (src_textures[0])
uniform float factor = 1.f;
uniform float alpha_factor = 1.f;

layout(location=0) out vec4 color;

void main()
{
    const vec4 in_color = texture(this_texture, uv);
    color = vec4(in_color.rgb * factor, in_color.a * alpha_factor);
}