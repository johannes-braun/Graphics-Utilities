#include "prototype_proxies.hpp"

namespace gfx {
inline namespace v1 {
namespace impl
{
vk_mesh_proxy::vk_mesh_proxy(device& gpu)
      : _device(&gpu)
      , _staging_vertex_buffer(gpu)
      , _staging_index_buffer(gpu)
      , _staging_bvh_buffer(gpu)
      , _vertex_buffer(gpu)
      , _index_buffer(gpu)
      , _bvh_buffer(gpu)
{}

std::tuple<gsl::span<vertex3d>, gsl::span<index32>, gsl::span<bvh<3>::node>> vk_mesh_proxy::data() const noexcept
{
    return {_staging_vertex_buffer, _staging_index_buffer, _staging_bvh_buffer};
}

void vk_mesh_proxy::resize_stages(ptrdiff_t delta_vertices, ptrdiff_t delta_indices, ptrdiff_t delta_bvh)
{
    if (delta_vertices > 0) _staging_vertex_buffer.resize(_staging_vertex_buffer.size() + delta_vertices);
    if (delta_indices > 0) _staging_index_buffer.resize(_staging_index_buffer.size() + delta_indices);
    if (delta_bvh > 0) _staging_bvh_buffer.resize(_staging_bvh_buffer.size() + delta_bvh);
}

void vk_mesh_proxy::update_buffers(bool vertices, bool indices, bool bvh)
{
    if (vertices) _vertex_buffer.update(_staging_vertex_buffer);
    if (indices) _index_buffer.update(_staging_index_buffer);
    if (bvh) _bvh_buffer.update(_staging_bvh_buffer);
}

const device& vk_mesh_proxy::get_device() const
{
    return *_device;
}

const buffer<vertex3d>& vk_mesh_proxy::vertex_buffer() const noexcept
{
    return _vertex_buffer;
}

const buffer<index32>& vk_mesh_proxy::index_buffer() const noexcept
{
    return _index_buffer;
}

const buffer<bvh<3>::node>& vk_mesh_proxy::bvh_buffer() const noexcept
{
    return _bvh_buffer;
}
}    // namespace impl
}    // namespace v1
}    // namespace gfx
