
out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
};

uniform sampler2D picture;
uniform mat4 view_projection;
uniform float scale = 4.f;
uniform vec3 offset = vec3(0);
uniform vec3 factor = vec3(1);

layout(location=0) out vec4 position;

void run()
{

}

void main()
{
    ivec2 size = textureSize(picture, 0);
    ivec2 pixel = ivec2(gl_VertexID / size.y, gl_VertexID % size.y);
    vec3 rgb_color = factor * texelFetch(picture, pixel, 0).rgb + offset;

    position = clamp(vec4(rgb_color, 1), vec4(0.f), vec4(1.f));

    gl_PointSize = 1.5f;

    gl_Position = view_projection * vec4(scale*position.xyz, 1);
}