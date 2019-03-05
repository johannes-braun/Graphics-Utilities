#pragma once
#include <array>
#include <atomic>
#include <iterator>
#include <list>
#include <omp.h>
#include <stack>
#include <thread>
#include <utility>

namespace gfx {
namespace detail {
struct bvh_range_t
{
    int32_t start;
    int32_t end;
    int32_t parent;
};
template<size_t Dimension>
struct temporaries_t
{
    using bvh_type = basic_bvh<Dimension>;
    std::atomic_int                          range_position;
    std::vector<typename bvh_type::vec_type> centroids;

    std::vector<bvh_range_t>                    index_ranges;
    std::vector<typename bvh_type::bounds_type> centroid_bounds;

    std::vector<bvh_range_t>                    swap_index_ranges;
    std::vector<typename bvh_type::bounds_type> swap_centroid_bounds;
};
}    // namespace detail

template<size_t Dimension>
template<typename It, typename GetFun, typename>
void basic_bvh<Dimension>::build_indexed(It begin, It end, GetFun&& get_vertex, size_t max_per_node)
{
    build_cache tmp {};
    build_indexed(tmp, begin, end, get_vertex, max_per_node);
}

template<size_t Dimension>
template<typename It, typename GetFun, typename>
void basic_bvh<Dimension>::build_indexed(build_cache& cache, It begin, It end, GetFun&& get_vertex, size_t max_per_node)
{
    const int count = int(std::distance(begin, end) / float(_points_per_shape));
    if (count <= 0) return;

    _node_count = 0;
    _depth      = 0;
    _nodes.clear();

    detail::temporaries_t<Dimension>& temporaries = cache.temporaries;
    temporaries.centroids.resize(count, vec_type(0));
    temporaries.index_ranges.resize(count);
    temporaries.centroid_bounds.resize(count);
    temporaries.swap_index_ranges.resize(count);
    temporaries.swap_centroid_bounds.resize(count);
    _nodes.resize(2 * count);

    temporaries.index_ranges[0] = {/*start=*/0, /*end=*/int(count - 1), /*parent=*/-1};

    for (int i = 0; i < count; ++i)
    {
        vec_type& centroid = temporaries.centroids[i];
        for (int c = 0; c < _points_per_shape; ++c)
            centroid += reinterpret_cast<vec_type&>(get_vertex(*std::next(begin, i * _points_per_shape + c)));
        centroid /= float(_points_per_shape);
        temporaries.centroid_bounds[0] += centroid;
    }

    _node_count = 0;
    temporaries.range_position.store(0);
    for (std::atomic_int i {1}; i.load() != 0;)
    {
        ++_depth;
        temporaries.range_position.store(0);
        const int c = i.load();
        for (int gid = 0; gid < c; ++gid) i += split_sah(begin, end, gid, std::forward<GetFun&&>(get_vertex), temporaries, max_per_node);

        _node_count += c;

        for (auto p = 0; p < i; ++p)
        {
            temporaries.index_ranges[p]    = std::move(temporaries.swap_index_ranges[p]);
            temporaries.centroid_bounds[p] = std::move(temporaries.swap_centroid_bounds[p]);
        }
    }

    temporaries.range_position.store(0);
    temporaries.centroids.resize(0);
    temporaries.index_ranges.resize(0);
    temporaries.centroid_bounds.resize(0);
    temporaries.swap_index_ranges.resize(0);
    temporaries.swap_centroid_bounds.resize(0);

    _nodes.resize(_node_count);
    _nodes.shrink_to_fit();
}

template<size_t Dimension>
std::vector<std::byte> pack(const basic_bvh<Dimension>& bvh, size_t vertex_stride, size_t vertex_offset, size_t index_stride,
                            size_t index_offset)
{
    using u32                   = uint32_t;
    u32                    vstr = u32(vertex_stride);
    u32                    voff = u32(vertex_offset);
    u32                    istr = u32(index_stride);
    u32                    ioff = u32(index_offset);
    std::vector<std::byte> packed_bytes(bvh.nodes().size() * sizeof(typename basic_bvh<Dimension>::node) + sizeof(u32) * 4);
    memcpy(packed_bytes.data() + 0 * sizeof(u32), &vstr, sizeof(u32));
    memcpy(packed_bytes.data() + 1 * sizeof(u32), &voff, sizeof(u32));
    memcpy(packed_bytes.data() + 2 * sizeof(u32), &istr, sizeof(u32));
    memcpy(packed_bytes.data() + 3 * sizeof(u32), &ioff, sizeof(u32));
    memcpy(packed_bytes.data() + 4 * sizeof(u32), bvh.nodes().data(), bvh.nodes().size() * sizeof(typename basic_bvh<Dimension>::node));
    return packed_bytes;
}

template<size_t Dimension>
const std::vector<typename basic_bvh<Dimension>::node>& basic_bvh<Dimension>::nodes() const noexcept
{
    return _nodes;
}

template<size_t Dimension>
basic_bvh<Dimension>::basic_bvh(std::uint16_t points_per_shape) : _points_per_shape(points_per_shape)
{}

template<size_t Dimension>
const typename basic_bvh<Dimension>::bounds_type& basic_bvh<Dimension>::get_bounds() const
{
    return _nodes[0].aabb;
}

template<size_t Dimension>
template<typename Leaf>
void basic_bvh<Dimension>::visit_leafs(Leaf&& leaf)
{
    for (const auto& n : _nodes)
        if (n.type == node_type::leaf) leaf(n);
}

template<size_t Dimension>
template<typename Inner, typename Leaf>
void basic_bvh<Dimension>::visit_all(Inner&& inner, Leaf&& leaf)
{
    for (const auto& n : _nodes)
        if (n.type == node_type::leaf)
            leaf(n);
        else
            inner(n);
}

template<size_t Dimension>
template<typename Cond, typename Comp, typename Leaf>
void basic_bvh<Dimension>::visit_conditional(Cond&& cond, Comp&& comp, Leaf&& leaf)
{
    if (_nodes.empty()) return;

    const node*             root_node    = _nodes.data();
    const node*             current_node = _nodes.data();
    std::stack<const node*> node_stack;

    const bool hits_scene = cond(*current_node);
    while (hits_scene)
    {
        while (current_node->type == node_type::inner)
        {
            const node* left       = root_node + current_node->child_left;
            const node* right      = root_node + current_node->child_right;
            bool        hits_left  = cond(*left);
            bool        hits_right = cond(*right);

            if (!hits_left && !hits_right) break;

            bool        nrm         = comp(*left, *right);
            const node* first       = nrm ? left : right;
            const node* second      = nrm ? right : left;
            bool        hits_first  = nrm ? hits_left : hits_right;
            const bool  hits_second = nrm ? hits_right : hits_left;

            if (hits_first && hits_second) node_stack.push(second);
            current_node = hits_first ? first : second;
        }

        if (current_node->type == node_type::leaf)
        {
            for (uint32_t i = current_node->child_left; i != current_node->child_right + 1; ++i)
            {
                if (leaf(*current_node)) return;
            }
        }

        if (node_stack.empty()) return;
        current_node = node_stack.top();
        node_stack.pop();
    }
}

template<size_t Dimension>
template<typename It, typename GetFun>
int basic_bvh<Dimension>::split_sah(It begin, It end, int gid, GetFun&& get_vertex, detail::temporaries_t<Dimension>& temporaries,
                                    size_t max_per_node) noexcept
{
    constexpr static int   bin_count   = 16;
    constexpr static int   plane_count = bin_count - 1;
    constexpr static float bin_epsilon = 0.01f;

    const detail::bvh_range_t& current_range  = temporaries.index_ranges[gid];
    const bounds_type&         current_bounds = temporaries.centroid_bounds[gid];
    const int                  candidates     = int(current_range.end + 1 - current_range.start);
    const int                  current_node   = int(gid + _node_count);

    if ((candidates > max_per_node || current_range.parent == -1) && !current_bounds.empty())
    {
        const int32_t range_index = temporaries.range_position.fetch_add(1);

        struct
        {
            float       cost        = std::numeric_limits<float>::max();
            float       centbox_min = 0;
            float       k           = 0;
            int         axis        = 0;
            int         plane       = 0;
            bounds_type left_bounds;
            bounds_type right_bounds;
        } best;

        struct bin
        {
            bounds_type aabb;
            int         objects = 0;
        };

        for (auto axis = 0; axis < Dimension; ++axis)
        {
            std::array<bin, bin_count> bins;

            auto cb_axis_min = current_bounds.min[axis];
            auto cb_axis_max = current_bounds.max[axis];

            const float k = bin_count * (1 - bin_epsilon) / (cb_axis_max - cb_axis_min);

            for (int candidate = current_range.start; candidate <= current_range.end; ++candidate)
            {
                const auto centroid = temporaries.centroids[candidate];
                const auto id       = static_cast<int32_t>(k * (centroid[axis] - cb_axis_min));
                bins[id].aabb += centroid;
                ++bins[id].objects;
            }

            std::array<bin, plane_count> bins_left;
            bins_left[0].aabb += bins[0].aabb;
            bins_left[0].objects = bins[0].objects;
            for (int plane = 1; plane < plane_count; ++plane)
            {
                bins_left[plane].aabb += bins_left[plane - 1].aabb;
                bins_left[plane].aabb += bins[plane].aabb;
                bins_left[plane].objects = bins_left[plane - 1].objects + bins[plane].objects;
            }

            std::array<bin, plane_count> bins_right;
            for (int plane = plane_count - 1; plane >= 0; --plane)
            {
                bins_right[plane].aabb += bins[plane + 1].aabb;
                bins_right[plane].objects = bins[plane + 1].objects;

                if (plane != plane_count - 1)
                {
                    bins_right[plane].aabb += bins_right[plane + 1].aabb;
                    bins_right[plane].objects += bins_right[plane + 1].objects;
                }

                const float surface_left  = bins_left[plane].aabb.surface();
                const float surface_right = bins_right[plane].aabb.surface();

                const float exponent = 2;
                const float cost     = std::pow(surface_left, exponent) * bins_left[plane].objects
                                   + std::pow(surface_right, exponent) * bins_right[plane].objects;
                if (cost < best.cost)
                {
                    best.cost         = cost;
                    best.axis         = axis;
                    best.plane        = plane;
                    best.centbox_min  = cb_axis_min;
                    best.k            = k;
                    best.left_bounds  = bins_left[plane].aabb;
                    best.right_bounds = bins_right[plane].aabb;
                }
            }
        }

        int  left          = current_range.start;
        int  right         = current_range.end;
        bool left_stopped  = false;
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
                // swap triangles and centroids
                for (int off = 0; off < int(_points_per_shape); ++off)
                    std::iter_swap(std::next(begin, int(_points_per_shape) * left + off),
                                   std::next(begin, int(_points_per_shape) * right + off));
                std::swap(temporaries.centroids[left], temporaries.centroids[right]);

                left_stopped  = false;
                right_stopped = false;
                ++left;
                --right;
            }
        }

        // determing left_range, right_range for the following threads
        detail::bvh_range_t left_range;
        detail::bvh_range_t right_range;
        left_range.start = current_range.start;
        right_range.end  = current_range.end;
        if (left > right)
        {
            left_range.end    = right;
            right_range.start = left;
        }
        else
        {
            if (left_stopped)
            {
                left_range.end    = left - 1;
                right_range.start = left;
            }
            else if (right_stopped)
            {
                left_range.end    = right;
                right_range.start = right + 1;
            }
            else
            {
                const int bin_id = int(best.k * (temporaries.centroids[left][best.axis] - best.centbox_min));

                if (bin_id > best.plane)
                {
                    left_range.end    = left - 1;
                    right_range.start = left;
                }
                else
                {
                    left_range.end    = left;
                    right_range.start = left + 1;
                }
            }
        }

        left_range.parent  = current_node;
        right_range.parent = current_node;

        // writing the id_ranges and centboxes to the buffers for processing in following threads
        temporaries.swap_index_ranges[range_index * 2]        = left_range;
        temporaries.swap_index_ranges[range_index * 2 + 1]    = right_range;
        temporaries.swap_centroid_bounds[range_index * 2]     = best.left_bounds;
        temporaries.swap_centroid_bounds[range_index * 2 + 1] = best.right_bounds;

        node node;
        for (int i = current_range.start; i <= current_range.end; ++i)
            for (int j = 0; j < int(_points_per_shape); ++j)
                node.aabb += reinterpret_cast<vec_type&>(get_vertex(*std::next(begin, i * int(_points_per_shape) + j)));

        node.type   = node_type::inner;
        node.parent = current_range.parent;

        // 5. writing to the node_buffer for traversal
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
            for (int j = 0; j < int(_points_per_shape); ++j)
                new_leaf.aabb += reinterpret_cast<vec_type&>(get_vertex(*std::next(begin, i * int(_points_per_shape) + j)));

        new_leaf.type        = node_type::leaf;
        new_leaf.child_left  = current_range.start;
        new_leaf.child_right = current_range.end;
        new_leaf.parent      = current_range.parent;

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
}    // namespace gfx