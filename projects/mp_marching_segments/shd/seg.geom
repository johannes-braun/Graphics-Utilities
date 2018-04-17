layout(points) in;
layout(line_strip, max_vertices = 4) out;

layout(location=0) in vec2 in_pixel[];


layout(binding = 0) uniform Data
{
    mat4 vp;
    sampler2D tex;
};


in gl_PerVertex
{
    vec4 gl_Position;
float gl_PointSize;
float gl_ClipDistance[];
} gl_in[];

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location=0) out vec2 pixel;

const vec4 luma = vec4(0.299, 0.587, 0.114, 0);
void main()
{
    vec4 color_00 = texelFetch(tex, ivec2(pixel), 0);
    vec4 color_01 = texelFetch(tex, ivec2(pixel) + ivec2(0, 1), 0);
    vec4 color_10 = texelFetch(tex, ivec2(pixel) + ivec2(1, 0), 0);
    vec4 color_11 = texelFetch(tex, ivec2(pixel) + ivec2(1, 1), 0);

    float val_00 = dot(luma, color_00);
    float val_01 = dot(luma, color_01);
    float val_10 = dot(luma, color_10);
    float val_11 = dot(luma, color_11);

    float v00_01 = (((val_01-val_00)/val_00));
    float v01_11 = ((val_01-val_11)/val_11);
    float v11_10 = ((val_11-val_10)/val_10);

    pixel = in_pixel[0];
    gl_Position = vp * vec4(
        gl_in[0].gl_Position.x + 1,
        gl_in[0].gl_Position.y,
        gl_in[0].gl_Position.z + v11_10,
        1);
    EmitVertex();
    gl_Position = vp * vec4(
        gl_in[0].gl_Position.x,
        gl_in[0].gl_Position.y,
        gl_in[0].gl_Position.z + v00_01,
        1);
    EmitVertex();
    EndPrimitive();
}