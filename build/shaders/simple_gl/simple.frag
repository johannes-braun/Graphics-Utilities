// Inputs
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 normal;

// Outputs
layout(location = 0) out vec4 color;

// Uniforms
uniform sampler2D image_texture;
uniform vec3 light_position;
uniform vec3 light_color;
uniform vec3 background;

void main()
{
    const vec3 to_light = light_position - position.xyz;
    const float dist_squared = dot(to_light, to_light);
    const vec3 light_tint = light_color * max(dot(normalize(to_light), normalize(normal.xyz)), 0.f) / (1+dist_squared);

    color = vec4(light_tint + background, 1) * texture(image_texture, uv);
}