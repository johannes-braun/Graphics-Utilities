layout(location=0) in vec2 uv;

uniform sampler2D src_textures[];
#define this_texture (src_textures[0])
#define other_texture (src_textures[1])

uniform float factor_one = 0.7f;
uniform float factor_two = 0.3f;

layout(location=0) out vec4 color;

void main()
{
    color = vec4(factor_one*texture(this_texture, uv).rgb, 1) + vec4(factor_two*texture(other_texture, uv).rgb, 0);
}