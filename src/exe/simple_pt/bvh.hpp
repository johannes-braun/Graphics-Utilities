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
#include <glm/glm.hpp>

namespace gfx
{
    enum class shape
    {
        point = 1,
        line = 2,
        triangle = 3,
    };

    enum class node_type : uint32_t
    {
        inner = 0,
        leaf
    };

    template<size_t Dimension>
    class bvh
    {
        struct range
        {
            int32_t start;
            int32_t end;
            int32_t parent;
        };
    public:
        static_assert(Dimension <= 3 && Dimension > 0, "Dimension must be 1, 2 or 3. For 3D vectors with one homogenous coordinate use 3 dimensions.");

        using byte = uint8_t;
        constexpr static size_t vec_size = std::clamp(1 << (Dimension - 1), 0, 4);
        using vec_type = glm::vec<vec_size, float, glm::packed_highp>;
        using vec_dim_type = glm::vec<Dimension, float, glm::packed_highp>;

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
            bool empty() const noexcept {
                return glm::any(glm::lessThanEqual(vec_dim_type(size()), vec_dim_type(std::numeric_limits<float>::epsilon())));
            }
            float surface() const noexcept { const vec_type s = size(); return 2.f * (s.x * s.y + s.x * s.z + s.y * s.z); }
        };

        struct bin
        {
            bounds bounds;
            int objects = 0;
        };

        struct node
        {
            bounds bounds;
            node_type type = node_type::leaf;
            int32_t child_left = -1;
            int32_t child_right = -1;
            int32_t parent = -1;
        };

        bvh(shape s, size_t vertex_stride, size_t vertex_offset, size_t index_stride, size_t index_offset)
            : _shape(s), _vertex_stride(vertex_stride), _vertex_offset(vertex_offset), _index_stride(index_stride), _index_offset(index_offset)
        {

        }

        template<typename It, typename GetFun, typename = std::enable_if_t<sizeof(decltype(std::declval<GetFun>()(*std::declval<It>()))) >= Dimension * sizeof(float)>>
        const std::vector<byte>& sort(It begin, It end, GetFun get_vertex)
        {
            _packed_bytes.clear();
            const int count = int(std::distance(begin, end) / float(_shape));
            if (count <= 0) return _packed_bytes;
            temporaries._get_vertex = [begin, get_vertex](size_t index) -> vec_type { return reinterpret_cast<vec_type&>(get_vertex(*std::next(begin, index))); };
            
            _node_count = 0;
            _depth = 0;
            _nodes.clear();

            temporaries.centroids.resize(count, vec_type(0));
            temporaries.index_ranges.resize(count);
            temporaries.centroid_bounds.resize(count);
            temporaries.swap_index_ranges.resize(count);
            temporaries.swap_centroid_bounds.resize(count);
            _nodes.resize(2 * count);

            temporaries.index_ranges[0] = { /*start=*/ 0, /*end=*/ int(count - 1), /*parent=*/ -1 };

            const int concurrency = std::thread::hardware_concurrency();
            std::vector<bounds> bounds(concurrency);

            #pragma omp parallel for schedule(static)
            for (int i = 0; i < count; ++i)
            {
                vec_type& centroid = temporaries.centroids[i];
                for (int c = 0; c < int(_shape); ++c)
                    centroid += temporaries._get_vertex(i * int(_shape) + c);
                centroid /= float(_shape);
                bounds[omp_get_thread_num()] += centroid;
            }
            for (const auto& b : bounds)
                temporaries.centroid_bounds[0] += b;

            _node_count = 0;
            temporaries.range_position.store(0);
            for (std::atomic_int i{ 1 }; i.load() != 0;)
            {
                ++_depth;
                temporaries.range_position.store(0);
                const int count = i.load();
                #pragma omp parallel for schedule(dynamic)
                for (int gid = 0; gid < count; ++gid)
                    i += split_sah(begin, end, gid);

                _node_count += count;

                #pragma omp parallel for schedule(static)
                for (auto p = 0; p < i; ++p)
                {
                    temporaries.index_ranges[p] = std::move(temporaries.swap_index_ranges[p]);
                    temporaries.centroid_bounds[p] = std::move(temporaries.swap_centroid_bounds[p]);
                }
            }

            temporaries.range_position.store(0);
            temporaries.centroids.resize(0);
            temporaries.index_ranges.resize(0);
            temporaries.centroid_bounds.resize(0);
            temporaries.swap_index_ranges.resize(0);
            temporaries.swap_centroid_bounds.resize(0);

            return pack();
        }

        const std::vector<byte>& get_packed() const noexcept { return _packed_bytes; }
        
        bvh(const bvh& other) { operator=(std::forward<const bvh&>(other)); }
        bvh& operator=(const bvh& other)
        {
            _nodes = other._nodes;
            _node_count = other._node_count;
            _depth = other._depth;
            _packed_bytes = other._packed_bytes;
            return *this;
        }

        bvh(bvh&& other) noexcept { operator=(std::forward<bvh&&>(other)); }
        bvh& operator=(bvh&& other) noexcept
        {
            _nodes = std::move(other._nodes);
            _node_count = other._node_count;
            _depth = other._depth;
            _packed_bytes = std::move(other._packed_bytes);
            return *this;
        }


    private:
        const std::vector<byte>& pack()
        {
            using u32 = uint32_t;
            u32 vstr = u32(_vertex_stride);
            u32 voff = u32(_vertex_offset);
            u32 istr = u32(_index_stride);
            u32 ioff = u32(_index_offset);

            _packed_bytes.resize(_node_count * sizeof(node) + sizeof(u32) * 4);
            memcpy(_packed_bytes.data() + 0 * sizeof(u32), &vstr, sizeof(u32));
            memcpy(_packed_bytes.data() + 1 * sizeof(u32), &voff, sizeof(u32));
            memcpy(_packed_bytes.data() + 2 * sizeof(u32), &istr, sizeof(u32));
            memcpy(_packed_bytes.data() + 3 * sizeof(u32), &ioff, sizeof(u32));
            memcpy(_packed_bytes.data() + 4 * sizeof(u32), _nodes.data(), _node_count * sizeof(node));
            return _packed_bytes;
        }

        template<typename It>
        int split_sah(It begin, It end, int gid) noexcept
        {
            constexpr static int bin_count = 16;
            constexpr static int plane_count = bin_count - 1;
            constexpr static float bin_epsilon = 0.01f;

            const range& current_range = temporaries.index_ranges[gid];
            const bounds& current_bounds = temporaries.centroid_bounds[gid];
            const int candidates = int(current_range.end + 1 - current_range.start);
            const int current_node = int(gid + _node_count);

            if ((candidates > 1 || current_range.parent == -1) && !current_bounds.empty())
            {
                const int32_t range_index = temporaries.range_position.fetch_add(1);

                struct {
                    float cost = std::numeric_limits<float>::max();
                    float centbox_min = 0;
                    float k = 0;
                    int axis = 0;
                    int plane = 0;
                    bounds left_bounds;
                    bounds right_bounds;
                } best;

                for (auto axis = 0; axis < Dimension; ++axis)
                {
                    std::array<bin, bin_count> bins;

                    auto cb_axis_min = current_bounds.min[axis];
                    auto cb_axis_max = current_bounds.max[axis];

                    const float k = bin_count * (1 - bin_epsilon) / (cb_axis_max - cb_axis_min);

                    for (int candidate = current_range.start; candidate <= current_range.end; ++candidate)
                    {
                        const auto centroid = temporaries.centroids[candidate];
                        const auto id = static_cast<int32_t>(k * (centroid[axis] - cb_axis_min));
                        bins[id].bounds += centroid;
                        ++bins[id].objects;
                    }

                    std::array<bin, plane_count> bins_left;
                    bins_left[0].bounds += bins[0].bounds;
                    bins_left[0].objects = bins[0].objects;
                    for (int plane = 1; plane < plane_count; ++plane)
                    {
                        bins_left[plane].bounds += bins_left[plane - 1].bounds;
                        bins_left[plane].bounds += bins[plane].bounds;
                        bins_left[plane].objects = bins_left[plane - 1].objects + bins[plane].objects;
                    }

                    std::array<bin, plane_count> bins_right;
                    for (int plane = plane_count - 1; plane >= 0; --plane)
                    {
                        bins_right[plane].bounds += bins[plane + 1].bounds;
                        bins_right[plane].objects = bins[plane + 1].objects;

                        if (plane != plane_count - 1)
                        {
                            bins_right[plane].bounds += bins_right[plane + 1].bounds;
                            bins_right[plane].objects += bins_right[plane + 1].objects;
                        }

                        const float surface_left = bins_left[plane].bounds.surface();
                        const float surface_right = bins_right[plane].bounds.surface();

                        const float exponent = 2;
                        const float cost = std::pow(surface_left, exponent) * bins_left[plane].objects + std::pow(surface_right, exponent) * bins_right[plane].objects;
                        if (cost < best.cost)
                        {
                            best.cost = cost;
                            best.axis = axis;
                            best.plane = plane;
                            best.centbox_min = cb_axis_min;
                            best.k = k;
                            best.left_bounds = bins_left[plane].bounds;
                            best.right_bounds = bins_right[plane].bounds;
                        }
                    }
                }

                int left = current_range.start;
                int right = current_range.end;
                bool left_stopped = false;
                bool right_stopped = false;
                 
                while (left < right)
                {
                    if (!left_stopped)
                    {
                        if (const int bin_id = int(best.k * (temporaries.centroids[left][best.axis] - best.centbox_min)); bin_id > best.plane)
                            left_stopped = true;
                        else
                            ++left;
                    }
                    if (!right_stopped)
                    {
                        if (const int bin_id = int(best.k * (temporaries.centroids[right][best.axis] - best.centbox_min)); bin_id <= best.plane)
                            right_stopped = true;
                        else
                            --right;
                    }
                    if (left_stopped && right_stopped)
                    {
                        //swap triangles and centroids
                        for (int off = 0; off < int(_shape); ++off)
                            std::iter_swap(std::next(begin, int(_shape)*left + off), std::next(begin, int(_shape)*right + off));
                        std::swap(temporaries.centroids[left], temporaries.centroids[right]);

                        left_stopped = false;
                        right_stopped = false;
                        ++left;
                        --right;
                    }
                }

                //determing left_range, right_range for the following threads
                range left_range;
                range right_range;
                left_range.start = current_range.start;
                right_range.end = current_range.end;
                if (left > right)
                {
                    left_range.end = right;
                    right_range.start = left;
                }
                else
                {
                    if (left_stopped)
                    {
                        left_range.end = left - 1;
                        right_range.start = left;
                    }
                    else if (right_stopped)
                    {
                        left_range.end = right;
                        right_range.start = right + 1;
                    }
                    else
                    {
                        const int bin_id = int(best.k * (temporaries.centroids[left][best.axis] - best.centbox_min));

                        if (bin_id > best.plane)
                        {
                            left_range.end = left - 1;
                            right_range.start = left;
                        }
                        else
                        {
                            left_range.end = left;
                            right_range.start = left + 1;
                        }
                    }
                }

                left_range.parent = current_node;
                right_range.parent = current_node;

                //writing the id_ranges and centboxes to the buffers for processing in following threads
                temporaries.swap_index_ranges[range_index * 2] = left_range;
                temporaries.swap_index_ranges[range_index * 2 + 1] = right_range;
                temporaries.swap_centroid_bounds[range_index * 2] = best.left_bounds;
                temporaries.swap_centroid_bounds[range_index * 2 + 1] = best.right_bounds;

                node node;
                for (int i = current_range.start; i <= current_range.end; ++i)
                    for (int j = 0; j < int(_shape); ++j)
                        node.bounds += temporaries._get_vertex(i * int(_shape) + j);

                node.type = node_type::inner;
                node.parent = current_range.parent;

                //5. writing to the node_buffer for traversal
                _nodes[current_node] = node;
                if (current_range.parent != -1)
                {
                    if (current_node % 2 == 0)
                        _nodes[current_range.parent].child_right = current_node;
                    else
                        _nodes[current_range.parent].child_left = current_node;
                }

                return 1;
            }
            else 
            {
                // The current node is a leaf node.
                auto&& new_leaf = _nodes[current_node];
                for (int i = current_range.start; i <= current_range.end; ++i)
                    for (int j = 0; j < int(_shape); ++j)
                        new_leaf.bounds += temporaries._get_vertex(i * int(_shape) + j);

                new_leaf.type = node_type::leaf;
                new_leaf.child_left = current_range.start;
                new_leaf.child_right = current_range.end;
                new_leaf.parent = current_range.parent;

                if (current_range.parent != -1)
                {
                    if (current_node % 2 == 0)
                        _nodes[new_leaf.parent].child_right = current_node;
                    else
                        _nodes[new_leaf.parent].child_left = current_node;
                }

                return -1;
            }
        }

        struct
        {
            std::atomic_int range_position;
            std::vector<vec_type> centroids;

            std::vector<range> index_ranges;
            std::vector<bounds> centroid_bounds;

            std::vector<range> swap_index_ranges;
            std::vector<bounds> swap_centroid_bounds;
            std::function<vec_type(size_t index)> _get_vertex;
        } temporaries;

        shape _shape;
        size_t _vertex_stride;
        size_t _vertex_offset;
        size_t _index_stride;
        size_t _index_offset;

        std::vector<node> _nodes;
        size_t _node_count;
        size_t _depth;
        std::vector<byte> _packed_bytes;
    };
}