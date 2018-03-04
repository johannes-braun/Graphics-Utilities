layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

uniform mat4 model_view_projection;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec4 out_color;

void main()
{
    out_color = color;
    gl_Position = model_view_projection * vec4(position, 1);
}