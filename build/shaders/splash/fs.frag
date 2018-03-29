layout(location = 0) in vec2 uv;
layout(location =1) in vec4 col;

layout(binding = 0, std140) uniform Data
{
    mat4 screen_mat;
    sampler2D img;
};

layout(location = 0) out vec4 color;

void main()
{
    color = col * texture(img, uv);
}

