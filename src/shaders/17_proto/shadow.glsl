
const float vps = 60.f;
const float shd_dist_div = 4.f / vps;
const int smoothing_size = 3;

layout(loc_gl(3) loc_vk(3, 0)) uniform sampler2DArrayShadow shadowmap;
struct shadow_cam_data
{
	mat4 view;
	mat4 proj;
	vec3 pos;
};
layout(loc_gl(2) loc_vk(3, 1)) uniform ShadowCamera
{
	shadow_cam_data data[256];
} shadow_camera;

vec3 get_pos(mat4 mat, vec3 pos)
{
    vec4 map_pos = mat * vec4(pos, 1);
    map_pos /= map_pos.w;
    map_pos.xy = 0.5f * map_pos.xy + 0.5f;
    return map_pos.xyz;
}

int get_layer(in sampler2DArrayShadow map, float dist, out float frac)
{
	ivec3 ts = textureSize(map, 0);
	float layers = ts.z;
	const int l = int(log2(int(dist *shd_dist_div + 0.5f)));
	frac = 0.f;
	if(l >= layers) return -1;
	frac = fract(log2(dist *shd_dist_div + 0.5f));
	return l;
}

float shadow(in sampler2DArrayShadow map, vec3 pos, float dist, vec3 normal, vec3 light_dir){
	ivec3 ts = textureSize(map, 0);
    vec2 tex_size = ts.xy;
	float layers = ts.z;
	
	float frac = 0.f;
	const int layer = get_layer(map, dist, frac);
	if(layer == -1)
		return 1.f;

	const mat4 mat = shadow_camera.data[layer].proj * shadow_camera.data[layer].view;
    vec3 map_pos = get_pos(mat, pos);

	const float layer_next = clamp(layer+1, 0, layers-1);

	const mat4 mat_next = layer_next == layer ? mat4(1.f) : shadow_camera.data[int(layer_next)].proj * shadow_camera.data[int(layer_next)].view;
    vec3 map_pos_next = get_pos(mat_next, pos);

    float shadow = 0.f;
    vec2 inv_size = 1/max(tex_size, vec2(1, 1));

    for(int i=0; i<smoothing_size*smoothing_size; ++i)
    {
        const int x = (i % smoothing_size - (smoothing_size >> 1)-1);
        const int y = (i / smoothing_size - (smoothing_size >> 1)-1);
        const vec2 offset = vec2(x, y) * inv_size;

        const vec2 uv = clamp(map_pos.xy + offset, vec2(0), vec2(1));
        const vec2 uv2 = clamp(map_pos_next.xy + offset, vec2(0), vec2(1));
        const float depth = 1-texture(map, vec4(get_uv(uv), layer, map_pos.z)).r;
        const float depth2 = layer_next == layer ? 1.f : 1-texture(map, vec4(get_uv(uv2), layer_next, map_pos_next.z)).r;

        shadow += mix(depth, depth2, frac);
    }
    return shadow / (smoothing_size*smoothing_size);
}
