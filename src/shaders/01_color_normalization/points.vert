#version 460 core

#ifdef VULKAN
#define loc_gl(N)
#define loc_vk(S, N) set = S, binding = N
#else
#define loc_gl(N) binding = N
#define loc_vk(S, N) 
#endif

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
};

layout(loc_gl(0) loc_vk(0, 1)) uniform sampler2D picture;

layout(loc_gl(0) loc_vk(0, 0)) uniform render_info
{
    mat4 view_projection;
    mat4 hat_mat;
    vec3 average;
};

layout(location=0) out vec4 position;

void main()
{
    ivec2 size = textureSize(picture, 0);
    ivec2 pixel = ivec2(gl_VertexID / size.y, gl_VertexID % size.y);
    vec3 rgb_color = /*factor * */texelFetch(picture, pixel, 0).rgb/* + offset*/;

    position = clamp(hat_mat * vec4(rgb_color, 1), vec4(0.f), vec4(1.f));

    gl_PointSize = 3.0f;

    gl_Position = view_projection * vec4(4.f*position.xyz, 1);
}
