layout(location=0) in vec2 uv;

uniform sampler2D tex;
uniform mat4 hat_mat = mat4(1.0f);

layout(location=0) out vec4 color;

void main()
{
    color = clamp(hat_mat * texture(tex, vec2(uv.x, 1-uv.y)), vec4(0), vec4(1));
    //color = clamp(vec4(factor, 1)*(texture(tex, vec2(uv.x, 1-uv.y)) + vec4(offset, 0)), vec4(0.f), vec4(1.f));
}