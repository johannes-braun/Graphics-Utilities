//? #version 460 core
//? #include "instance.glsl"
//? #include "geometry.glsl"
//? #include "def.hpp"

#ifndef LAYOUTS_DEFINED
#define layout_buffer_binding_models set = 1, binding = 0
#define layout_buffer_binding_vertex set = 1, binding = buffer_binding_vertex
#define layout_buffer_binding_element set = 1, binding = buffer_binding_element
#define layout_buffer_binding_bvh set = 1, binding = buffer_binding_bvh
#endif

layout(set = 0, binding = 0) uniform Camera
{
	mat4 view;
	mat4 proj;
	vec3 position;
} camera;

layout(std430, layout_buffer_binding_models) restrict readonly buffer Models
{
	basic_instance models[];
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

void main()
{
	light_t lights[2];

	lights[0].color = 0xcceeffff;
	lights[0].intensity = 50.f;
	lights[0].position = vec3(5, 5, 5);

	lights[1].color = 0xffeeccff;
	lights[1].intensity = 50.f;
	lights[1].position = vec3(-3, 4, -6);
	// ------------------------------------------------

	color = vec4(0);

	// Diffuse Lighting (Lambert)
	for(int id = 0; id < lights.length(); ++id)
	{
		light_t light = lights[id];
		const vec3 to_light = light.position - position;
		const float distance_to_light_2 = dot(to_light, to_light);
		const vec3 norm_to_light = normalize(to_light);
		float cosTheta = max(dot(normalize(normal), norm_to_light), 0);
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
				color += get_color(models[draw_index]) * unpack_rgba8(light.color) * light.intensity * cosTheta * attenuation;
		 
				const vec3 half_vector = normalize(normalize(to_light) - normalize(position - camera.position));
				const float cosThetaBlinn = max(dot(half_vector, normalize(normal)), 0);
				color += light.intensity * pow(cosThetaBlinn, 70) * attenuation * cosTheta;
			}
		}
	}
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