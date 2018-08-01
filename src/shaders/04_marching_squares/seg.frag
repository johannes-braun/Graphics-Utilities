#version 460 core

layout(location = 0) in vec2 pixel;
layout(location = 1) in vec4 in_color;

layout(binding = 0) uniform Data
{
    mat4 vp;
};

layout(binding = 0) uniform sampler2D tex;

layout(location = 0) out vec4 color;

void main()
{
    color = in_color;// texelFetch(tex, ivec2(pixel), 0);
}