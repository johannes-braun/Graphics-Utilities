#version 460 core

out gl_PerVertex
{
	vec4 gl_Position;
};

const vec3 poses[] = {
	vec3(0, 0, 0),
	vec3(0, 1, 0),
	vec3(1, 0, 0)
};

void main()
{
	gl_Position = vec4(poses[gl_VertexID], 1);
}