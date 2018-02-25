layout(location=0) in vec2 uv;

uniform sampler2D src_textures[];
#define this_texture (src_textures[0])
#define other_texture (src_textures[1])

uniform float factor_one = 0.7f;
uniform float factor_two = 0.3f;

layout(location=0) out vec4 color;

void main()
{
    const vec4 color_lhs = texture(this_texture, uv);
    const vec4 color_rhs = texture(other_texture, uv);

    color = vec4(factor_one*color_lhs.rgb, color_lhs.a) + vec4(factor_two*color_rhs.rgb, color_rhs.a);
}