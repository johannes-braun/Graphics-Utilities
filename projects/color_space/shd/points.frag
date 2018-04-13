layout(location=0) in vec4 world_position;

layout(location = 0) out vec4 color;

void main()
{
    color = vec4(world_position.rgb, 1);
}