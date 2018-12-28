#ifndef DEF_HPP
#define DEF_HPP

#ifndef __cplusplus

#define CONSTEXPR const
#define INIT_AS_1(A)
#define INIT_AS_2(A, B)
#define INIT_AS_3(A, B, C)
#define INIT_AS_4(A, B, C, D)
#define u8vec4 uint
#define NAMESPACE_START(N)
#define NAMESPACE_END(N)

#else

#define CONSTEXPR constexpr
#include <cstdint>
#include <glm/glm.hpp>
namespace def {
using namespace glm;
using uint = std::uint32_t;
#define INIT_AS_1(A) \
    {                \
        (A)          \
    }
#define INIT_AS_2(A, B) \
    {                   \
        (A), (B)        \
    }
#define INIT_AS_3(A, B, C) \
    {                      \
        (A), (B), (C)      \
    }
#define INIT_AS_4(A, B, C, D) \
    {                         \
        (A), (B), (C), (D)    \
    }
#define NAMESPACE_START(N) namespace N {
#define NAMESPACE_END(N) }

#endif

#define DEF_use_rt_shadows 0

CONSTEXPR uint buffer_binding_vertex  = 1;
CONSTEXPR uint buffer_binding_element = 2;
CONSTEXPR uint buffer_binding_bvh     = 3;

CONSTEXPR uint texture_binding_all = 0;

CONSTEXPR uint constant_id_texture_count = 0;

CONSTEXPR uint shadow_map_binding = 100;
CONSTEXPR uint shadow_cam_binding = 4;

#ifndef __cplusplus
// specialization constants
layout(constant_id = constant_id_texture_count) const uint MAX_TEXTURE_COUNT = 164;

layout(layout_texture_binding_all) uniform sampler2D all_textures[MAX_TEXTURE_COUNT];

// helpers
vec2 remap_uv(const in vec2 uv)
{
    return vec2(uv.x, 1 - uv.y);
}
#endif

struct sampled_color_t
{
    u8vec4 color INIT_AS_4(255, 50, 0, 255);
    int texture_id INIT_AS_1(-1);
};

struct mesh_info
{
    sampled_color_t diffuse;

    int bump_map_texture_id INIT_AS_1(-1);
};

#ifndef __cplusplus
vec4 unpack_rgba8(uint c)
{
    return vec4(((c >> 24) & 0xff) / 255.f, ((c >> 16) & 0xff) / 255.f, ((c >> 8) & 0xff) / 255.f, ((c >> 0) & 0xff) / 255.f);
}

vec4 sample_color(const in sampled_color_t c, vec2 uv)
{
    if (c.texture_id >= 0) { return texture(all_textures[c.texture_id], remap_uv(uv)); }
    return unpack_rgba8(c.color);
};

// defined in "frag.base.glsl"
mat3 cotangentFrame(vec3 N, vec3 p, vec2 uv);
vec3 from_bump(vec3 surf_pos, vec3 surf_norm, vec2 uv, vec3 view_dir, int bumpmap_id, float bscale)
{
    if (bumpmap_id == -1) return surf_norm;

#ifdef USE_OPENGL
    bscale *= -1;
#endif

    mat3 tbn = cotangentFrame(surf_norm, surf_pos, uv);

    float height = bscale * (texture(all_textures[bumpmap_id], uv).r - 0.5f);

    // The following does weird stuff
    // uv += height * (view_dir * tbn).xy;

    vec2 duv = 1.f / vec2(textureSize(all_textures[bumpmap_id], 0));
    vec4 h   = bscale
             * vec4(texture(all_textures[bumpmap_id], uv + vec2(duv.x, 0)).r, texture(all_textures[bumpmap_id], uv + vec2(-duv.x, 0)).r,
                    texture(all_textures[bumpmap_id], uv + vec2(0, duv.y)).r, texture(all_textures[bumpmap_id], uv + vec2(0, -duv.y)).r)
             / duv.xxyy;

    return tbn * normalize(cross(vec3(2, 0, h[0] - h[1]), vec3(0, 2, h[2] - h[3])));
}

#endif

#ifdef __cplusplus
}    // namespace def
#endif

#undef CONSTEXPR
#undef INIT_AS_1
#undef INIT_AS_2
#undef INIT_AS_3
#undef INIT_AS_4
#endif    // DEF_HPP