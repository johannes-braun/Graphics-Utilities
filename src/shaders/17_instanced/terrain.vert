#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 chunk_position;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = vec4(chunk_position.x + position.x, 0, chunk_position.y + position.y, 1);
}