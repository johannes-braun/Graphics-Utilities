layout(location=0) in vec2 uv;

uniform sampler2D src_textures[];
#define this_texture (src_textures[0])

uniform float threshold_lower = 1.f;
uniform float threshold_higher = 1.2f;
const vec3 luma = vec3(0.299f, 0.587f, 0.114f);

layout(location=0) out vec4 color;

void main()
{
    const vec3 tex_color = pow(texture(this_texture, uv).rgb, vec3(1/2.2f));
    color = vec4(smoothstep(threshold_lower, threshold_higher, dot(luma, tex_color)) * tex_color, 1);
}