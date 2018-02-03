layout(location=0) in vec2 uv;

uniform vec4 tint = vec4(1, 1, 1, 1);
uniform sampler2D tex;

layout(location = 0) out vec4 color;

void main()
{
    if(tint.a == -1)
    {
        color = texture(tex, uv);
        color.a = (1-color.r) * 0.8f;
        color.rgb = vec3(0.4f);
    }
    else
    {
        color = texture(tex, uv) * tint;
    }
}