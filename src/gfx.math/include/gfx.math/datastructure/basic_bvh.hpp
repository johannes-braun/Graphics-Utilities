#pragma once

#include <algorithm>
#include <glm/glm.hpp>
#include <vector>

#include "../geometry.hpp"

namespace gfx {
namespace detail {
template<size_t Dimension>
struct temporaries_t;
}

enum class node_type : uint32_t
{
    inner = 0,
    leaf
};

template<size_t Dimension>
class basic_bvh
{
public:
    friend struct detail::temporaries_t<Dimension>;
    static_assert(Dimension <= 3 && Dimension > 0,
                  "Dimension must be 1, 2 or 3. For 3D vectors with one homogenous coordinate use 3 dimensions.");

    using vec_type     = glm::vec<Dimension, float, glm::packed_highp>;
    using vec_dim_type = glm::vec<Dimension, float, glm::packed_highp>;
    using bounds_type  = gfx::bounds<float, Dimension, 4 * std::clamp<size_t>(1ll << (Dimension - 1), 0, 4)>;

    struct node
    {
        bounds_type aabb;
        node_type   type        = node_type::leaf;
        int32_t     child_left  = -1;
        int32_t     child_right = -1;
        int32_t     parent      = -1;
    };

    explicit basic_bvh(std::uint16_t points_per_shape);
    basic_bvh(const basic_bvh& other) = default;
    basic_bvh& operator=(const basic_bvh& other) = default;
    basic_bvh(basic_bvh&& other) noexcept        = default;
    basic_bvh& operator=(basic_bvh&& other) noexcept = default;
    ~basic_bvh()                                     = default;

    template<typename It, typename GetFun>
    using enable_if_getter_valid =
        std::enable_if_t<sizeof(decltype(std::declval<GetFun>()(*std::declval<It>()))) >= sizeof(vec_type)>;

    template<typename It, typename GetFun, typename = enable_if_getter_valid<It, GetFun>>
    void build_indexed(It begin, It end, GetFun&& get_vertex, size_t max_per_node = 1);

    auto nodes() const noexcept -> std::vector<node> const&;
    auto get_bounds() const -> bounds_type const&;

    template<typename Fun, typename = decltype(std::declval<Fun>()(node{}))>
    void visit(Fun&& fun);

    template<typename Cond, typename Comp, typename Leaf>
    void visit_conditional(Cond&& cond, Comp&& comp, Leaf&& leaf);

private:
    template<typename It, typename GetFun>
    int split_sah(It begin, It end, int gid, GetFun&& get_vertex, detail::temporaries_t<Dimension>& temporaries,
                  size_t max_per_node) noexcept;

    std::vector<node> _nodes;
    std::uint16_t     _points_per_shape;
    size_t            _node_count = 0;
    size_t            _depth      = 0;
};

using bvh1d = basic_bvh<1>;
using bvh2d = basic_bvh<2>;
using bvh3d = basic_bvh<3>;

template<size_t Dimension>
std::vector<std::byte> pack(const basic_bvh<Dimension>& bvh, size_t vertex_stride, size_t vertex_offset, size_t index_stride,
                            size_t index_offset);
}    // namespace gfx

#include "basic_bvh.inl"