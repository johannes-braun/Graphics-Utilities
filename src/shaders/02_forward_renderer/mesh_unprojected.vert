layout(location=0) in vec3 position;
layout(location=1) in vec2 uv;
layout(location=2) in vec3 normal;

struct instance_info
{
    uint count;
    uint instance_count;
    uint base_index;
    uint base_vertex;
    uint base_instance;
    uint vertex_count;
    uint id;

    vec3 bounds_min;
    vec3 bounds_max;

    mat4 model_matrix;
    vec3 color;
    float roughness;

	uvec2 diffuse_texture;
	uvec2 bump_texture;
};

layout(binding = 10, std430) readonly buffer ModelData
{
    instance_info instances[];
};

out gl_PerVertex 
{
    vec4 gl_Position;
};

layout(location=0) out vec3 out_position;
layout(location=1) out vec2 out_uv;
layout(location=2) out vec3 out_normal;
layout(location=3) flat out uint draw_id;

void main()
{
    draw_id = gl_DrawID;
    out_position = (instances[gl_DrawID].model_matrix*vec4(position, 1)).xyz;
    out_uv = uv;
    out_normal = normalize((inverse(transpose(instances[gl_DrawID].model_matrix)) * vec4(normal, 1)).xyz);

    gl_Position = instances[gl_DrawID].model_matrix * vec4(position, 1);
}