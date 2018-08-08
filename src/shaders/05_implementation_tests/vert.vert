#version 460 core

out gl_PerVertex
{
	vec4 gl_Position;
};

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(-0.5, 0.5),
    vec2(0.5, 0.5)
);

layout(location=0) out vec4 pos;

void main()
{
	gl_Position = vec4(positions[gl_VertexID], 0, 1);
	pos = gl_Position;
}