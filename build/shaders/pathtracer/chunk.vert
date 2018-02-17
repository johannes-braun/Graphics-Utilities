out gl_PerVertex
{
    vec4 gl_Position;
};

uniform vec2 inv_resolution;
uniform vec2 offset;
uniform vec2 size;

const vec2 positions[4] = vec2[](
    vec2(0, 0),
    vec2(0, 1),
    vec2(1, 1),
    vec2(1, 0)
);

void main()
{
    int vid = int(gl_VertexID);
    gl_Position.zw = vec2(0, 1);
    gl_Position.xy = ((offset + positions[vid % 4] * size) * vec2(2.f) * inv_resolution - vec2(1.f)).xy;
}