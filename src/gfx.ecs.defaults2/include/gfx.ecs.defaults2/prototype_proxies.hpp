#pragma once
#include "prototype.hpp"
#include <gfx.graphics/graphics.hpp>
#include <gfx.math/datastructure/bvh.hpp>
#include <gfx.math/geometry.hpp>
#include <gsl/span>

namespace gfx {
inline namespace v1 {
namespace vulkan {
class device;
class mesh_proxy : public mesh_allocator::proxy
{
public:
    mesh_proxy(device& gpu);
    std::tuple<gsl::span<vertex3d>, gsl::span<index32>, gsl::span<bvh<3>::node>> data() const noexcept override;
    void resize_stages(ptrdiff_t delta_vertices, ptrdiff_t delta_indices, ptrdiff_t delta_bvh) override;
    void update_buffers(bool vertices, bool indices, bool bvh) override;

    const device&               get_device() const;
    const buffer<vertex3d>&     vertex_buffer() const noexcept;
    const buffer<index32>&      index_buffer() const noexcept;
    const buffer<bvh<3>::node>& bvh_buffer() const noexcept;

private:
    device* _device;

    mapped<vertex3d>     _staging_vertex_buffer;
    mapped<index32>      _staging_index_buffer;
    mapped<bvh<3>::node> _staging_bvh_buffer;

    buffer<vertex3d>     _vertex_buffer;
    buffer<index32>      _index_buffer;
    buffer<bvh<3>::node> _bvh_buffer;
};

template<typename InstanceInfo>
class instance_proxy : public prototype_instantiator<InstanceInfo>::proxy, public mesh_proxy
{
public:
    constexpr static size_t instance_swap_buffer_count = 2;
    using typename prototype_instantiator<InstanceInfo>::proxy::range_type;
    using basic_instance = typename prototype_instantiator<InstanceInfo>::basic_instance;

    instance_proxy(device& gpu);
    void                                             free_range(const range_type& range) override;
    std::pair<range_type, gsl::span<basic_instance>> allocate_range(size_t count) override;
    void                                             clear() override;

    void                          swap(commands& cmd);
    const mapped<basic_instance>& instances_mapped() const noexcept;
    const buffer<basic_instance>& instances_buffer() const noexcept;
    size_t                        instance_buffer_index() const noexcept;

private:
    size_t                 _current_instance_index = 0;
    mapped<basic_instance> _instance_descriptions_src[instance_swap_buffer_count];
    buffer<basic_instance> _instance_descriptions_dst[instance_swap_buffer_count];
};
}    // namespace impl


namespace opengl {
class device;
class mesh_proxy : public mesh_allocator::proxy
{
public:
    mesh_proxy();
    std::tuple<gsl::span<vertex3d>, gsl::span<index32>, gsl::span<bvh<3>::node>> data() const noexcept override;
    void resize_stages(ptrdiff_t delta_vertices, ptrdiff_t delta_indices, ptrdiff_t delta_bvh) override;
    void update_buffers(bool vertices, bool indices, bool bvh) override;

    const buffer<vertex3d>&     vertex_buffer() const noexcept;
    const buffer<index32>&      index_buffer() const noexcept;
    const buffer<bvh<3>::node>& bvh_buffer() const noexcept;

private:
    mapped<vertex3d>     _staging_vertex_buffer;
    mapped<index32>      _staging_index_buffer;
    mapped<bvh<3>::node> _staging_bvh_buffer;

    buffer<vertex3d>     _vertex_buffer;
    buffer<index32>      _index_buffer;
    buffer<bvh<3>::node> _bvh_buffer;
};

template<typename InstanceInfo>
class instance_proxy : public prototype_instantiator<InstanceInfo>::proxy, public mesh_proxy
{
public:
    constexpr static size_t instance_swap_buffer_count = 2;
    using typename prototype_instantiator<InstanceInfo>::proxy::range_type;
    using basic_instance = typename prototype_instantiator<InstanceInfo>::basic_instance;

    instance_proxy();
    void                                             free_range(const range_type& range) override;
    std::pair<range_type, gsl::span<basic_instance>> allocate_range(size_t count) override;
    void                                             clear() override;

    void                          swap();
    const mapped<basic_instance>& instances_mapped() const noexcept;
    const buffer<basic_instance>& instances_buffer() const noexcept;
    size_t                        instance_buffer_index() const noexcept;

private:
    size_t                 _current_instance_index = 0;
    mapped<basic_instance> _instance_descriptions_src[instance_swap_buffer_count];
    buffer<basic_instance> _instance_descriptions_dst[instance_swap_buffer_count];
};
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx

#include "prototype_proxies.inl"