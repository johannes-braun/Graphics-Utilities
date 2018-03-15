#pragma once

#include <iterator>
#include <vector>
#include <array>
#include <set>
#include <numeric>

namespace geometry
{
    template<size_t Dim>
    class bvh
    {
        template<typename It> using is_valid_iterator = std::void_t<
            typename std::iterator_traits<It>::difference_type,
            typename std::iterator_traits<It>::pointer,
            typename std::iterator_traits<It>::reference,
            typename std::iterator_traits<It>::value_type,
            typename std::iterator_traits<It>::iterator_category>;

    public:
        struct vec { float x = 0, y = 0, z = 0, w = 0; float& operator[](int idx) { return reinterpret_cast<float*>(this)[idx]; } };
        struct bounds
        {
            vec min{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
            vec max{ std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };
        };
        struct node
        {
            bounds b;
            uint32_t type = 1;

            // Hierarchial info
            int32_t begin = -1;
            int32_t end = -1;
            int32_t parent = -1;
        };

        bvh(int primitive_size)
            : _prim_size(primitive_size)
        {

        }

        template<typename It, typename Getter,
            typename = std::void_t<is_valid_iterator<It>, decltype(std::iter_swap(std::declval<It>(), std::declval<It>()))>>
            void sort(It begin, It end, Getter getter)
        {
            const size_t size = end - begin;
            if (size == 0)
                return;
            nodes.resize(2 * size / _prim_size + 3);
            sort_impl(begin, begin, end, getter, -1, 0);
        };

        const std::vector<node>& my_nodes() const
        {
            return nodes;
        }

    private:

        template<typename It, typename Getter>
        int sort_impl(It start, It begin, It end, Getter getter, int par, int rec)
        {
            const auto get_pos = [&](const decltype(*begin)& iter) {
                auto v = getter(iter);
                vec out;
                memcpy(&out, &v, std::clamp(std::min(sizeof(out), sizeof(v)), 0ui64, Dim * sizeof(float)));
                return out;
            };

            const size_t size = end - begin;
            if (size == 0)
                return 0;

            std::vector<bounds> prim_bounds(size / _prim_size);
            std::vector<vec> prim_centers(size / _prim_size);
            bounds center_bounds;
            bounds node_bounds;
            for (auto it = begin; it != end; it = it + _prim_size)
            {
                const size_t index = (it - begin) / _prim_size;
                bounds& b = prim_bounds[index];
                for (int i = 0; i < _prim_size; ++i)
                {
                    vec v = get_pos(*(it + i));
                    b.min.x = std::min(v.x, b.min.x);
                    b.min.y = std::min(v.y, b.min.y);
                    b.min.z = std::min(v.z, b.min.z);
                    b.min.w = std::min(v.w, b.min.w);

                    b.max.x = std::max(v.x, b.max.x);
                    b.max.y = std::max(v.y, b.max.y);
                    b.max.z = std::max(v.z, b.max.z);
                    b.max.w = std::max(v.w, b.max.w);

                    node_bounds.min.x = std::min(v.x, node_bounds.min.x);
                    node_bounds.min.y = std::min(v.y, node_bounds.min.y);
                    node_bounds.min.z = std::min(v.z, node_bounds.min.z);
                    node_bounds.min.w = std::min(v.w, node_bounds.min.w);

                    node_bounds.max.x = std::max(v.x, node_bounds.max.x);
                    node_bounds.max.y = std::max(v.y, node_bounds.max.y);
                    node_bounds.max.z = std::max(v.z, node_bounds.max.z);
                    node_bounds.max.w = std::max(v.w, node_bounds.max.w);
                }

                vec& center = prim_centers[index];
                center.x = 0.5f*(b.max.x + b.min.x);
                center.y = 0.5f*(b.max.y + b.min.y);
                center.z = 0.5f*(b.max.z + b.min.z);
                center.w = 0.5f*(b.max.w + b.min.w);

                center_bounds.min.x = std::min(center.x, center_bounds.min.x);
                center_bounds.min.y = std::min(center.y, center_bounds.min.y);
                center_bounds.min.z = std::min(center.z, center_bounds.min.z);
                center_bounds.min.w = std::min(center.w, center_bounds.min.w);

                center_bounds.max.x = std::max(center.x, center_bounds.max.x);
                center_bounds.max.y = std::max(center.y, center_bounds.max.y);
                center_bounds.max.z = std::max(center.z, center_bounds.max.z);
                center_bounds.max.w = std::max(center.w, center_bounds.max.w);
            }
            if (size == _prim_size)
            {
                nodes[rec].b = node_bounds;
                nodes[rec].type = 1;
                nodes[rec].begin = (begin - start) / _prim_size;
                nodes[rec].end = (end - 1 - start) / _prim_size;
                nodes[rec].parent = par;
                return 1;
            }

            vec bounds_size;
            bounds_size.x = center_bounds.max.x - center_bounds.min.x;
            bounds_size.y = center_bounds.max.y - center_bounds.min.y;
            bounds_size.z = center_bounds.max.z - center_bounds.min.z;
            bounds_size.w = center_bounds.max.w - center_bounds.min.w;

            int largest = 0;
            float current = bounds_size.x;
            if constexpr (Dim >= 2)
            {
                if (bounds_size.y > current)
                    largest = 1;
            }
            if constexpr (Dim >= 3)
            {
                if (bounds_size.z > current)
                    largest = 2;
            }
            if constexpr (Dim >= 4)
            {
                if (bounds_size.w > current)
                    largest = 3;
            }

            for (auto it = begin; it != end- _prim_size; it+= _prim_size)
            {
                const size_t index = (it - begin) / _prim_size;
                const float& center = prim_centers[index][largest];

                for (auto ins = it + _prim_size; ins != end; ins += _prim_size)
                {
                    const size_t index2 = (ins - begin) / _prim_size;
                    const float& center2 = prim_centers[index2][largest];

                    if (center < center2)
                    {
                        std::iter_swap(it, ins);
                        std::iter_swap(it+1, ins+1);
                        std::iter_swap(it+2, ins+2);
                    }
                }
            }
            
      /*      using T = decltype(*begin);
            std::sort(begin, end, [&](const uint32_t& lidx, const uint32_t& ridx) {
                auto diff = & (*begin) - &lidx;
                return prim_centers[int(&lidx - &(*begin))/3][largest] < prim_centers[int(&ridx - &(*begin)) / 3][largest];
            });*/

            const size_t center = ((size / _prim_size) / 2) * _prim_size;

            nodes[rec].b = node_bounds;
            nodes[rec].type = 0;
            nodes[rec].begin = rec + 1;
            nodes[rec].parent = par;

            const int count = sort_impl(start, begin, begin + center, getter, rec, nodes[rec].begin);
            nodes[rec].end = rec + count + 1;
            return count + sort_impl(start, begin + center, end, getter, rec, nodes[rec].end) + 1;
        }

        std::vector<node> nodes;
        int _prim_size;
    };
}