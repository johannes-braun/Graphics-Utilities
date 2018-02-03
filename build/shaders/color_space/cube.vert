layout(location = 0) in vec4 position;
layout(location = 1) in vec2 uv;

out gl_PerVertex
{
    vec4 gl_Position;
};

uniform mat4 view_projection;
uniform float scale = 4.f;

layout(location = 0) out vec2 out_uv;

void main()
{
    gl_Position = view_projection * (vec4(vec3(scale * 0.5f), 1.f) * (position*vec4(vec3(1.005f), 1.f) + vec4(1,1,1, 0.f)));
    out_uv = uv; 
}