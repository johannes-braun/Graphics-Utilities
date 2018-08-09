#pragma once

namespace std
{
template<>
struct hash<gfx::vertex3d>
{
    size_t operator()(const gfx::vertex3d& vertex) const
    {
        const hash<glm::vec3> v3h;
        const hash<uint32_t>  u32h;
        const auto            hash_position      = v3h(vertex.position);
        const auto            hash_normal        = v3h(vertex.normal);
        const auto            hash_uv            = hash<glm::vec2>()(vertex.uv);
        const auto            hash_meta_position = u32h(vertex.metadata_position);
        const auto            hash_meta_normal   = u32h(vertex.metadata_normal);
        const auto            hash_meta_uv       = hash<uint64_t>()(vertex.metadata_uv);

        return (hash_position << 2 ^ hash_normal << 1) >> 1 ^ (hash_uv ^ hash_meta_position >> 1) << 1
               ^ (hash_meta_normal >> 2 ^ hash_meta_uv >> 3) << 2;
    }
};
template<>
struct hash<gfx::vertex2d>
{
    size_t operator()(const gfx::vertex2d& vertex) const
    {
        const hash<glm::vec2> v2h;
        const auto            hash_position = v2h(vertex.position);
        const auto            hash_uv       = v2h(vertex.uv);
        const auto            hash_color    = hash<glm::u8vec4>()(vertex.color);

        return (hash_position << 2 ^ hash_uv << 1) >> 1 ^ hash_color;
    }
};
template<>
struct hash<gfx::transform>
{
    size_t operator()(const gfx::transform& transform) const
    {
        const hash<glm::vec2> v2h;
        const auto            hash_position = v2h(transform.position);
        const auto            hash_scale    = v2h(transform.scale);
        const auto            hash_rotation = hash<glm::quat>()(transform.rotation);

        return (hash_position << 2 ^ hash_scale << 1) >> 1 ^ hash_rotation;
    }
};

template<typename T, size_t Dim, size_t Align>
struct hash<gfx::bounds<T, Dim, Align>>
{
    size_t operator()(const gfx::bounds<T, Dim, Align>& bounds) const
    {
        const hash<typename gfx::bounds<T, Dim, Align>::value_type> vt;
        return (vt(bounds.min) << 1 ^ vt(bounds.max));
    }
};
}