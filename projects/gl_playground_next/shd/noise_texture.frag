layout(location = 0) in vec4 position;
layout(location = 1) flat in mat4 inv_vp;

layout(binding = 0) uniform Camera
{
    mat4 view;
    mat4 projection;
    vec3 camera_position;
};

struct Bounds
{
    vec3 min;
    vec3 max;
};

bool intersectBounds(const vec3 origin, const vec3 direction, const in Bounds bounds,
                     const float maxDistance, inout float outMinDistance, inout float outMaxDistance)
{
    vec3 invDirection = 1.f / direction;

    // intersections with box planes parallel to x, y, z axis
    vec3 t135 = (bounds.min - origin) * invDirection;
    vec3 t246 = (bounds.max - origin) * invDirection;

    vec3 minValues = min(t135, t246);
    vec3 maxValues = max(t135, t246);

    float tmin = max(max(minValues.x, minValues.y), minValues.z);
    float tmax = min(min(maxValues.x, maxValues.y), maxValues.z);

    outMinDistance = min(tmin, tmax);
    outMaxDistance = max(tmin, tmax);
    return tmax >= 0 && tmin <= tmax && tmin <= maxDistance;
}

layout(binding = 0) uniform sampler3D noise_texture;

layout(location=0) out vec4 color;

void main()
{
    vec4 flatPosition = projection * position;
    flatPosition.xy /= flatPosition.w;
    vec2 uv = flatPosition.xy;

    vec3 ro = camera_position.xyz;
    vec4 UV = inv_vp * vec4(uv, 0, 1);
    UV /= UV.w;
    vec3 rd = normalize(UV.xyz - ro);

    Bounds bounds;
    bounds.min = vec3(0);
    bounds.max = vec3(1);
    float tmin, tmax;
    if(!intersectBounds(ro, rd, bounds, 1.f/0.f, tmin, tmax))
    {
        discard;
    }

    tmin = max(tmin, 0);
    const int steps = 128;
    const float diff = tmax - tmin;
    const float dstep = diff / steps;
    float density = 0.f;
    float t = tmin;

    vec3 offx = vec3(1, 0, 0) / 512;
    vec3 offy = vec3(1, 0, 0) / 512;
    vec3 offz = vec3(1, 0, 0) / 512;

    float lighting = -1.f;
    vec3 tint = vec3(1.f);
    for(int i=0; i<steps; ++i)
    {
        const vec3 pos = (ro + t * rd);
        float dens000 = texture(noise_texture, pos).r;
        density += 2.f * dens000 * dstep;
        t += dstep;
    }

    float dens_dx = dFdx(density);
    float dens_dy = dFdy(density);

    color = vec4(tint, density);
}