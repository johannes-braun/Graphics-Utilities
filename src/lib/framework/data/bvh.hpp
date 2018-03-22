#pragma once

#include <iterator>
#include <array>
#include <vector>
#include <algorithm>
#include <functional>
#include <thread>
#include <atomic>
#include <utility>
#include <omp.h>
#include <stack>
#include <glm/glm.hpp>

namespace gfx
{
    enum class shape
    {
        point = 1,
        line = 2,
        triangle = 3,
    };

    enum class bvh_mode
    {
        gpu_oriented = 0, // default setting, keeps does not keep _get_vertex alive, so cannot be used for cpu-based traversal.
        persistent_iterators, // Keep _get_vertex and associated index iterators and captured variables alive. Enables cpu-based traversal, but needs more cautious usage.
    };

    enum class node_type : uint32_t
    {
        inner = 0,
        leaf
    };

    template<size_t Dimension>
    class bvh
    {
    public:
        struct hit_result
        {
            struct shape_index
            {
                uint32_t index;
                float barycentric_value;
            };

            operator bool() const noexcept { return hits; }

            std::list<shape_index> indices;
            float distance;
            bool hits;
        };

        static_assert(Dimension <= 3 && Dimension > 0, "Dimension must be 1, 2 or 3. For 3D vectors with one homogenous coordinate use 3 dimensions.");

        using byte = uint8_t;
        constexpr static size_t vec_size = std::clamp(1 << (Dimension - 1), 0, 4);
        using vec_type = glm::vec<vec_size, float, glm::packed_highp>;
        using vec_dim_type = glm::vec<Dimension, float, glm::packed_highp>;
        using intersect_function_type = std::function<hit_result(hit_result last, int index, const std::list<vec_dim_type>&)>;

        struct bounds
        {
            bounds() : min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::lowest()) {}

            union {
                vec_type data[2];
                struct {
                    vec_type min;
                    vec_type max;
                };
            };

            vec_type& operator[](size_t i) { return data[i]; }
            const vec_type& operator[](size_t i) const { return data[i]; }
            bounds& operator+=(const vec_type& v) { min = glm::min(min, v); max = glm::max(max, v); return *this; }
            bounds& operator+=(const bounds& b) { min = glm::min(min, b.min); max = glm::max(max, b.max); return *this; }
            vec_type size() const noexcept { return max - min; }
            bool empty() const noexcept { return glm::any(glm::lessThanEqual(vec_dim_type(size()), vec_dim_type(std::numeric_limits<float>::epsilon()))); }
            float surface() const noexcept { const vec_type s = size(); return 2.f * (s.x * s.y + s.x * s.z + s.y * s.z); }
        };

        struct node
        {
            bounds bounds;
            node_type type = node_type::leaf;
            int32_t child_left = -1;
            int32_t child_right = -1;
            int32_t parent = -1;
        };

        bvh(shape s, bvh_mode mode = bvh_mode::gpu_oriented);
        bvh(const bvh& other);
        bvh& operator=(const bvh& other);
        bvh(bvh&& other) noexcept;
        bvh& operator=(bvh&& other) noexcept;

        template<typename It, typename GetFun, typename = std::enable_if_t<sizeof(decltype(std::declval<GetFun>()(*std::declval<It>()))) >= Dimension * sizeof(float)>>
        void sort(It begin, It end, GetFun get_vertex);

        const std::vector<byte>& pack(size_t vertex_stride, size_t vertex_offset, size_t index_stride, size_t index_offset);
        const std::vector<byte>& get_packed() const noexcept;

        bounds get_bounds() const;

    private:
        bool intersect_ray_bounds(const vec_dim_type origin, const vec_dim_type direction, const bounds& bounds, const float max_distance, float* tmin) const noexcept;

    public:
        hit_result intersect_ray(const glm::vec3& origin, const glm::vec3& direction, const float max_distance, bool any = false) const;
        hit_result intersect_ray_with(const glm::vec3& origin, const glm::vec3& direction, const float max_distance, const intersect_function_type& intersect_function, bool any = false) const;

    private:
        template<typename It>
        int split_sah(It begin, It end, int gid) noexcept;

        struct range
        {
            int32_t start;
            int32_t end;
            int32_t parent;
        };

        struct
        {
            std::atomic_int range_position;
            std::vector<vec_type> centroids;

            std::vector<range> index_ranges;
            std::vector<bounds> centroid_bounds;

            std::vector<range> swap_index_ranges;
            std::vector<bounds> swap_centroid_bounds;
        } temporaries;

        shape _shape;
        bvh_mode _mode;

        intersect_function_type _custom_intersect;
        std::function<vec_type(size_t index)> _get_vertex;
        std::vector<node> _nodes;
        size_t _node_count;
        size_t _depth;
        std::vector<byte> _packed_bytes;
    };
}

#include "bvh.inl"