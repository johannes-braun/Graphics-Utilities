#version 460 core

layout(location=0) out vec2 uv;

void main()
{
    gl_Position = vec4(mix(-1, 3, gl_VertexID & 0x1), mix(-1, 3, (gl_VertexID >> 1) & 0x1), 0.f, 1.f);
    uv = ((gl_Position.xy+1)*0.5f);
	uv.y = 1-uv.y;
}