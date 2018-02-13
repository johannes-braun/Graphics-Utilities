
layout(location=0) in vec3 uv;

uniform samplerCube map;
uniform vec4 tint = vec4(1);
uniform float lod = 0;

layout(location=0) out vec4 fragment_color;

void main()
{
	fragment_color = tint * vec4(textureLod(map, uv, lod).rgb, 1);
}
