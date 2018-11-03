#version 460 core
#include <gfx.shaders/api.glsl> //! #include "../../../src/gfx.shaders/include/gfx.shaders/api.glsl"

layout(location=0) in vec2 uv;

layout(loc_gl(0) loc_vk(0, 1)) uniform sampler2D tex;

layout(location = 0) out vec4 color;

void main()
{
	color = vec4((1.f - 0.85f*texture(tex, uv)).rgb, 1.f);
}