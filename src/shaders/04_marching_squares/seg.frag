#version 460 core
#include "../api.glsl"

layout(location = 0) in vec2 pixel;
layout(location = 1) in vec4 in_color;

layout(loc_gl(0) loc_vk(0, 1)) uniform Data
{
    mat4 vp;
};

layout(loc_gl(0) loc_vk(0, 0)) uniform sampler2D tex;

layout(location = 0) out vec4 color;

void main()
{
    color = in_color;// texelFetch(tex, ivec2(pixel), 0);
}