//? #version 460 core
//? #include "instance.glsl"
//? #include "geometry.glsl"
//? #include "def.hpp"
#include "frag.base.glsl"

#ifndef LAYOUTS_DEFINED
#define layout_buffer_binding_models set = 1, binding = 0
#define layout_buffer_binding_vertex set = 1, binding = buffer_binding_vertex
#define layout_buffer_binding_element set = 1, binding = buffer_binding_element
#define layout_buffer_binding_bvh set = 1, binding = buffer_binding_bvh
#define layout_shadow_map_binding set = 1, binding = shadow_map_binding
#define layout_shadow_cam_binding set = 1, binding = shadow_cam_binding
#endif

layout(set = 0, binding = 0) uniform Camera
{
	camera_t camera;
};

layout(std430, layout_buffer_binding_models) restrict readonly buffer Models
{
	basic_instance models[];
};

layout(layout_shadow_map_binding) uniform sampler2DShadow shadow_map;
layout(layout_shadow_cam_binding) restrict readonly buffer ShadowMapCameras
{
	camera_t light_cameras[];
};

#if DEF_use_rt_shadows
layout(std430, layout_buffer_binding_vertex) restrict readonly buffer ModelVertices
{
	vertex_t model_vertices[];
};
layout(std430, layout_buffer_binding_element) restrict readonly buffer ModelElements
{
	uint model_elements[];
};
layout(std430, layout_buffer_binding_bvh) restrict readonly buffer ModelBVH
{
	bvh_node_t model_bvh_nodes[];
};
#endif

layout(location = 0) in vec3 normal;
layout(location = 1) flat in int draw_index;
layout(location = 2) in vec3 position; 
layout(location = 3) in vec2 uv;

const uint bvh_node_type_inner = 0;
const uint bvh_node_type_leaf  = 1;
const uint bvh_mode_any      = 0;
const uint bvh_mode_nearest  = 1;

struct bvh_result
{
    // The index of the nearest and farthest triangles.
    // (multiply by 3 to get the element index)
    uint near_triangle;

    float near_distance;

    // Only two barycentric coordinates are needed.
    // Calculate the last one with z = 1 - bary.x - bary.y .
    vec2 near_barycentric;
	uint instance;
    bool hits;
};

bvh_result bvh_hit(const vec3 origin, const vec3 direction, const float max_distance);
bvh_result bvh_hit(vec3 origin, vec3 direction, const float max_distance, uint root_node);
bvh_result bvh_hit_instanced(const vec3 origin, const vec3 direction, const float max_distance);
void       bvh_state_set_mode(uint mode);

layout(location = 0) out vec4 color;

struct light_t
{
	uint color;
	float intensity;
	vec3 position;
};

vec2 deep_parallax( 
    int displacement_texture_id, 
    const in vec3 view_tangent_space,
    const in vec2 base_uv,
    int min_layers, 
    int max_layers, 
    float depth_scale)
{
    float num_layers = mix(float(max_layers), float(min_layers), abs(dot(vec3(0, 0, 1), view_tangent_space)));
    float layer_depth = 1.0 / num_layers; 
    float current_layer_depth = 0.0; 
    vec2 P = view_tangent_space.xy * depth_scale; 
    vec2 delta = P / num_layers; 
    vec2 ofs = base_uv;  

    float depth = texture(all_textures[displacement_texture_id], remap_uv(ofs)).r;
    float current_depth = 0.0;
    int i = 0;
    while(current_depth < depth && i++ < max_layers) {
        ofs -= delta;
        depth = texture(all_textures[displacement_texture_id], remap_uv(ofs)).r;
        current_depth += layer_depth;
    }
    vec2 prev_ofs = ofs + delta;
    float after_depth  = depth - current_depth;
    float before_depth = texture(all_textures[displacement_texture_id], remap_uv(prev_ofs)).r - current_depth + layer_depth;
    float weight = after_depth / (after_depth - before_depth);
    return mix(ofs,prev_ofs,weight); 
}

const int smoothing_size = 5;
vec3 get_pos(mat4 mat, vec3 pos)
{
    vec4 map_pos = mat * vec4(pos, 1);
    map_pos /= map_pos.w; 
    map_pos.xy = 0.5f * map_pos.xy + 0.5f;
    return map_pos.xyz;
}

vec3 shadow(in camera_t cam, vec3 pos){
	ivec2 ts = textureSize(shadow_map, 0);
    vec2 tex_size = ts.xy;
	
	const mat4 mat = cam.proj * cam.view;
    vec3 map_pos = get_pos(mat, pos);

    float shadow = 0.f;
    vec2 inv_size = 1/max(tex_size, vec2(1, 1));

    for(int i=0; i<smoothing_size*smoothing_size; ++i)
    {
        const int x = (i % smoothing_size - (smoothing_size >> 1)-1);
        const int y = (i / smoothing_size - (smoothing_size >> 1)-1);
        const vec2 offset = vec2(x, y) * inv_size;

        const vec2 uv = clamp(map_pos.xy + offset, vec2(0), vec2(1));
        const float depth = 1-texture(shadow_map, vec3(remap_uv(uv), map_pos.z + 0.0001)).r;

        shadow += depth;
    }
    return vec3(1)*shadow / (smoothing_size*smoothing_size);
}

void main()
{
	light_t lights[2];

	lights[0].color = 0xcceeffff;
	lights[0].intensity = 20.f;
	lights[0].position = vec3(5, 5, 5);

	lights[1].color = 0xffeeccff;
	lights[1].intensity = 20.f;
	lights[1].position = vec3(-3, 5.5, -6);
	// ------------------------------------------------

	color = vec4(0);
	const vec3 v = normalize(camera.position - position);

	const float depth = -0.014f;

	const vec2 new_uv = remap_uv(deep_parallax( 
		models[draw_index].info.bump_map_texture_id,
		inverse(cotangentFrame(normalize(normal), position, uv)) * -v,
		uv,
		2, 
		16, 
		-depth));

	const vec3 real_normal = normalize(from_bump(position, normalize(normal), new_uv, normalize(position - camera.position), models[draw_index].info.bump_map_texture_id, depth));
	
	// Diffuse Lighting (Lambert)
	for(int id = 0; id < lights.length(); ++id)
	{
		light_t light = lights[id];
		const vec3 to_light = light.position - position;
		const float distance_to_light_2 = dot(to_light, to_light);
		const vec3 norm_to_light = normalize(to_light);
		float cosTheta = max(dot(real_normal, norm_to_light), 0);
		const float attenuation = 1/distance_to_light_2;
		if(cosTheta > 0)
		{
			bvh_state_set_mode(bvh_mode_any);
			bvh_result trace;
			trace.hits = false;
			if(bool(DEF_use_rt_shadows))
				trace = bvh_hit_instanced(position + 1e-3*norm_to_light, norm_to_light, sqrt(distance_to_light_2));

			if(!trace.hits)
			{
				const float roughness = 0.4f;
				const vec3 diffuse = sample_color(models[draw_index].info.diffuse, new_uv).rgb;

				const float alpha = max(roughness * roughness, 1e-3f);
				const float alpha2 = alpha * alpha;

				const vec3 n =  real_normal;
				const vec3 m = normalize(normalize(to_light) + v);
				const vec3 l = normalize(to_light);
				const float ndotm = max(dot(n, m), 0);
				const float ndotm2 = ndotm*ndotm;
				const float vdotm = max(dot(v, m), 0);
				const float vdotn = max(dot(v, n), 0);
				const float ldotn = max(dot(l, n), 0);
				
				const float pi = 3.14159265359f;
				const float inner_denom = ndotm2 * (alpha2 - 1) + 1;
				const float dggx = alpha2 / (pi * inner_denom * inner_denom);

				float f0 = (1-1.56f) / (1+1.56f);
				f0 *= f0;
				const float fschlick = f0 + (1-f0) * pow(1-vdotm, 5.f);

				const float gggxv = vdotn + sqrt( (vdotn - vdotn * alpha2) * vdotn + alpha2 );
				const float gggxl = ldotn + sqrt( (ldotn - ldotn * alpha2) * ldotn + alpha2 );
				const float gggx4aso = 1 / max(gggxv * gggxl, 0.0001f);

				const float brdf = dggx * fschlick * gggx4aso;

				color += light.intensity * brdf * attenuation;
				color += sample_color(models[draw_index].info.diffuse, uv) * unpack_rgba8(light.color) * light.intensity * cosTheta * attenuation;
			}
		}
	}
	const float shadowed = shadow(light_cameras[0], position).r;
	const vec4 light_pos = inverse(light_cameras[0].view) * vec4(0, 0, 0, 1);
	const vec3 light_pos3 = light_pos.xyz / light_pos.w;
	const vec3 to_sun = normalize(light_pos3 - position);
	const float cosThetaLight = dot(to_sun, real_normal);
	color += shadowed * sample_color(models[draw_index].info.diffuse, uv) * 1.1f * cosThetaLight;
}











































uint       _bvh_mode_current = bvh_mode_nearest;
void       bvh_state_set_mode(uint mode)
{
    _bvh_mode_current = mode;
}

bool intersect_bounds(const vec3 origin, const vec3 direction,

                      const vec3 bounds_min, const vec3 bounds_max, const float max_distance)
{
    vec3 inv_direction = 1.f / direction;

    // intersections with box planes parallel to x, y, z axis
    vec3 t135 = (bounds_min - origin) * inv_direction;
    vec3 t246 = (bounds_max - origin) * inv_direction;

    vec3 min_values = min(t135, t246);
    vec3 max_values = max(t135, t246);

    float tmin = max(max(min_values.x, min_values.y), min_values.z);
    float tmax = min(min(max_values.x, max_values.y), max_values.z);

    return tmax >= 0 && tmin <= tmax && tmin <= max_distance;
}

bool intersect_bounds(const vec3 origin, const vec3 direction,

                      const vec3 bounds_min, const vec3 bounds_max, const float max_distance, inout float min_distance)
{
    vec3 inv_direction = 1.f / direction;

    // intersections with box planes parallel to x, y, z axis
    vec3 t135 = (bounds_min - origin) * inv_direction;
    vec3 t246 = (bounds_max - origin) * inv_direction;

    vec3 min_values = min(t135, t246);
    vec3 max_values = max(t135, t246);

    float tmin = max(max(min_values.x, min_values.y), min_values.z);
    float tmax = min(min(max_values.x, max_values.y), max_values.z);

    min_distance = min(tmin, tmax);
    return tmax >= 0 && tmin <= tmax && tmin <= max_distance;
}


bool intersect_triangle(const vec3 origin, const vec3 direction, const vec3 v1, const vec3 v2, const vec3 v3, inout float t,
                        inout vec2 barycentric)
{
    float float_epsilon  = 1e-23f;
    float border_epsilon = 1e-6f;

    // Find vectors for two edges sharing V1
    vec3 e1 = v2 - v1;
    vec3 e2 = v3 - v1;

    // if determinant is near zero, ray lies in plane of triangle
    vec3  P   = cross(vec3(direction), e2);
    float det = dot(e1, P);
    if (det > -float_epsilon && det < float_epsilon) return false;

    // Calculate u parameter and test bound
    float inv_det = 1.f / det;
    vec3  T       = origin.xyz - v1;
    barycentric.x = dot(T, P) * inv_det;

    // The intersection lies outside of the triangle
    if (barycentric.x < -border_epsilon || barycentric.x > 1.f + border_epsilon) return false;

    // Calculate V parameter and test bound
    vec3 Q        = cross(T, e1);
    barycentric.y = dot(vec3(direction), Q) * inv_det;
    // The intersection lies outside of the triangle
    if (barycentric.y < -border_epsilon || barycentric.x + barycentric.y > 1.f + border_epsilon) return false;

    return (t = dot(e2, Q) * inv_det) > float_epsilon;
}

bvh_result bvh_hit_instanced(const vec3 origin, const vec3 direction, const float max_distance)
{
	bvh_result hit;
	hit.hits = false;
	float md = max_distance;
	for(int i=0; i<models.length(); ++i)
	{
		bvh_result nh = bvh_hit(origin, direction, md, i);
		if(nh.hits)
		{
			if (_bvh_mode_current == bvh_mode_any)
				return nh;
			hit = nh;
			md = hit.near_distance;
			hit.instance = i;
		}
	}
	return hit;
}

bvh_result bvh_hit(const vec3 origin, const vec3 direction, const float max_distance)
{
	return bvh_hit(origin, direction, max_distance, 0);	
}
bvh_result bvh_hit(vec3 origin, vec3 direction, const float max_distance, uint instance_id)
{
    bvh_result result;
    result.near_distance   = max_distance;
    result.hits            = false;
    float current_distance = 0;

#if !DEF_use_rt_shadows
	return result;
#else
	basic_instance inst = models[instance_id];
	mat4 inv_tf = inverse(inst.transform);
	origin		= (inv_tf * vec4(origin, 1)).xyz;
	direction	= ((inv_tf * vec4(direction, 0)).xyz);
	uint root_node = inst.base_bvh_node;

    bvh_node_t current_node = model_bvh_nodes[root_node];
    uint     current_id   = root_node;
    bool     hits_scene   = intersect_bounds(origin, direction, current_node.bounds_min.xyz, current_node.bounds_max.xyz, max_distance);

    uint bitstack    = 0;
    uint switchstack = 0;

    while (hits_scene)
    {
        while (current_node.type == bvh_node_type_inner)
        {
            uint  left      = current_node.left;
            uint  right     = current_node.right;
            float min_left  = 1.f / 0.f;
            float min_right = 1.f / 0.f;
            bool hits_left = intersect_bounds(origin, direction, model_bvh_nodes[root_node + left].bounds_min.xyz, model_bvh_nodes[root_node + left].bounds_max.xyz,
                                              result.near_distance, min_left);
            bool hits_right = intersect_bounds(origin, direction, model_bvh_nodes[root_node + right].bounds_min.xyz,
                                               model_bvh_nodes[root_node + right].bounds_max.xyz, result.near_distance, min_right);

            if (!hits_left && !hits_right) break;

            bool nrm         = min_left < min_right;
            uint first       = nrm ? left : right;
            uint second      = nrm ? right : left;
            bool hits_first  = nrm ? hits_left : hits_right;
            bool hits_second = nrm ? hits_right : hits_left;

            switchstack  = (switchstack << 1) | int(nrm);    // 0x1 -> right next, 0x0 -> left next
            bitstack     = (bitstack << 1) | int(hits_left && hits_right);
            current_node = model_bvh_nodes[root_node + (current_id = hits_first ? first : second)];
        }

        if (current_node.type == bvh_node_type_leaf)
        {
            vec2 current_barycentric;
            uint start = current_node.left;
            uint end   = current_node.right;
            for (uint i = start; i != end + 1; ++i)
            {
                vec3 tv1 = model_vertices[inst.base_vertex + model_elements[inst.base_index + 3 * i + 0]].position;
                vec3 tv2 = model_vertices[inst.base_vertex + model_elements[inst.base_index + 3 * i + 1]].position;
                vec3 tv3 = model_vertices[inst.base_vertex + model_elements[inst.base_index + 3 * i + 2]].position;

                if (intersect_triangle(origin, direction, tv1, tv2, tv3, current_distance, current_barycentric)
                    && current_distance < result.near_distance)
                {
                    result.hits             = true;
                    result.near_distance    = current_distance;
                    result.near_barycentric = current_barycentric;
                    result.near_triangle    = i;

                    if (_bvh_mode_current == bvh_mode_any) return result;
                }
            }
        }

        while ((bitstack & 1) == 0)
        {
            if (bitstack == 0) return result;

            current_id  = model_bvh_nodes[root_node + current_id].parent;
            bitstack    = bitstack >> 1;
            switchstack = switchstack >> 1;
        }

        current_id = ((switchstack & 0x1) == 0x1) ? model_bvh_nodes[root_node + model_bvh_nodes[root_node + current_id].parent].right
                                                  : model_bvh_nodes[root_node + model_bvh_nodes[root_node + current_id].parent].left;

        current_node = model_bvh_nodes[root_node + current_id];
        bitstack     = bitstack ^ 1;
    }
    return result;
#endif // !DEF_use_rt_shadows
}