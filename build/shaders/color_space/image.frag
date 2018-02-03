layout(location=0) in vec2 uv;

uniform vec3 offset = vec3(0);
uniform vec3 factor = vec3(1);
uniform sampler2D tex;

layout(location=0) out vec4 color;

void main()
{
    color = clamp(vec4(factor, 1)*(texture(tex, vec2(uv.x, 1-uv.y)) + vec4(offset, 0)), vec4(0.f), vec4(1.f));
}