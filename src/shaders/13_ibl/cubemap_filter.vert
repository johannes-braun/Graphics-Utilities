#version 460 core

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(location = 0) flat out int id;

void main()
{
	id = gl_VertexID;
	gl_Position = vec4(0, 0, 0, 1);
}