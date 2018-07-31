#version 460 core

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

in gl_PerVertex
{
	vec4 gl_Position;
} gl_in[];

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(location = 0) in vec4 color[];
layout(location = 1) in vec2 vel[];
layout(location = 0) out vec4 out_color;
layout(location = 1) out vec2 out_uv;

layout(binding = 0) uniform sampler2D img;

void main()
{
	vec4 c = gl_in[0].gl_Position;
	out_color = color[0];

	vec2 is = vec2(textureSize(img, 0));
	const vec2 s = 0.03f * vec2(1, is.y / is.x);

	int imin = vel[0].x < 0 ? 0 : 1;
	int imax = vel[0].x < 0 ? 1 : 0;
	
	gl_Position = vec4(c.x-s.x, c.y-s.y, 0, 1);
	out_uv = vec2(imin, 1);
	EmitVertex();
	gl_Position = vec4(c.x+s.x, c.y-s.y, 0, 1);
	out_uv = vec2(imax, 1);
	EmitVertex();
	gl_Position = vec4(c.x-s.x, c.y+s.y, 0, 1);
	out_uv = vec2(imin, 0);
	EmitVertex();
	gl_Position = vec4(c.x+s.x, c.y+s.y, 0, 1);
	out_uv = vec2(imax, 0);
	EmitVertex();
	EndPrimitive();
}