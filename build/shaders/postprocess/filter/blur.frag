
layout(location=0) in vec2 uv;

uniform sampler2D src_textures[];
#define this_texture (src_textures[0])
uniform int axis = 0;
uniform int level = 0;

const int half_kernel_size = 7;
const float half_kernel[half_kernel_size+1] = float[](
    0.001803,
	0.006045,
	0.016826,
	0.038879,
	0.074574,
	0.118748,
	0.156982,
	0.172288
);

layout(location=0) out vec4 color;


void main()
{
    ivec2 tex_size = textureSize(this_texture, level);
    vec2 axis_vector = vec2(0, 0);
    axis_vector[axis] = 1.f / tex_size[axis];

    for(int i=0; i<half_kernel_size; ++i)
    {
        color += half_kernel[i] * textureLod(this_texture, uv + (half_kernel_size - i) * axis_vector, level);
        color += half_kernel[i] * textureLod(this_texture, uv - (half_kernel_size - i) * axis_vector, level);
    }
    color = vec4(vec3(color + half_kernel[half_kernel_size] * textureLod(this_texture, uv, level)), 1);
}