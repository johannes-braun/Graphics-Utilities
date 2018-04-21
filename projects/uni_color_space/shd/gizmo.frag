layout(location=0) flat in int axis;

layout(location=0) out vec4 color;

void main()
{
    color = vec4(0, 0, 0, 1);
    color[axis] = 1;
}