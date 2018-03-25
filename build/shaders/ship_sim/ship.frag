layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec4 color;

void main()
{
    color = (max(dot(normal, normalize(vec3(1))), 0.f) + vec4(0.5f, 0.9f, 1.f, 1.f)) * vec4(0.4, 0.2, 0.f, 1);
}