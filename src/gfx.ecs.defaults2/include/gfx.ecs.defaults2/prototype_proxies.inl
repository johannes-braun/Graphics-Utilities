#pragma once

namespace gfx {
inline namespace v1 {
namespace vulkan {
template<typename InstanceInfo>
instance_proxy<InstanceInfo>::instance_proxy(device& gpu)
      : mesh_proxy(gpu)
      , _instance_descriptions_src {mapped<basic_instance> {gpu, 1}, mapped<basic_instance> {gpu, 1}}
{}

template<typename InstanceInfo>
void instance_proxy<InstanceInfo>::free_range(const typename instance_proxy<InstanceInfo>::range_type& range)
{
    const auto beg_it = std::next(_instance_descriptions_src[_current_instance_index].begin(), range.first);
    const auto end_it = std::next(_instance_descriptions_src[_current_instance_index].begin(), range.second);
    _instance_descriptions_src[_current_instance_index].erase(beg_it, end_it);
}

template<typename InstanceInfo>
auto instance_proxy<InstanceInfo>::allocate_range(size_t count) -> std::pair<range_type, gsl::span<basic_instance>>
{
    const auto s = _instance_descriptions_src[_current_instance_index].size();
    range_type r(s, s + count);
    _instance_descriptions_src[_current_instance_index].resize(s + count);
    gsl::span<basic_instance> span(_instance_descriptions_src[_current_instance_index].data() + s, count);
    return std::make_pair(r, span);
}

template<typename InstanceInfo>
void instance_proxy<InstanceInfo>::clear()
{
    _instance_descriptions_src[_current_instance_index].clear();
    _instance_descriptions_src[_current_instance_index].emplace_back();
}

template<typename InstanceInfo>
void instance_proxy<InstanceInfo>::swap(commands& cmd)
{
    _current_instance_index = (_current_instance_index + 1) % instance_swap_buffer_count;
}

template<typename InstanceInfo>
const mapped<typename prototype_instantiator<InstanceInfo>::basic_instance>& instance_proxy<InstanceInfo>::instances_mapped() const noexcept
{
    return _instance_descriptions_src[_current_instance_index];
}

template<typename InstanceInfo>
size_t instance_proxy<InstanceInfo>::instance_buffer_index() const noexcept
{
    return _current_instance_index;
}
}    // namespace impl

    
namespace opengl {
template<typename InstanceInfo>
instance_proxy<InstanceInfo>::instance_proxy()
      : mesh_proxy()
      , _instance_descriptions_src {mapped<basic_instance> {1}, mapped<basic_instance> {1}}
{}

template<typename InstanceInfo>
void instance_proxy<InstanceInfo>::free_range(const typename instance_proxy<InstanceInfo>::range_type& range)
{
    const auto beg_it = std::next(_instance_descriptions_src[_current_instance_index].begin(), range.first);
    const auto end_it = std::next(_instance_descriptions_src[_current_instance_index].begin(), range.second);
    _instance_descriptions_src[_current_instance_index].erase(beg_it, end_it);
}

template<typename InstanceInfo>
auto instance_proxy<InstanceInfo>::allocate_range(size_t count) -> std::pair<range_type, gsl::span<basic_instance>>
{
    const auto s = _instance_descriptions_src[_current_instance_index].size();
    range_type r(s, s + count);
    _instance_descriptions_src[_current_instance_index].resize(s + count);
    gsl::span<basic_instance> span(_instance_descriptions_src[_current_instance_index].data() + s, count);
    return std::make_pair(r, span);
}

template<typename InstanceInfo>
void instance_proxy<InstanceInfo>::clear()
{
    _instance_descriptions_src[_current_instance_index].clear();
    _instance_descriptions_src[_current_instance_index].emplace_back();
}

template<typename InstanceInfo>
void instance_proxy<InstanceInfo>::swap()
{
    _current_instance_index = (_current_instance_index + 1) % instance_swap_buffer_count;
}

template<typename InstanceInfo>
const mapped<typename prototype_instantiator<InstanceInfo>::basic_instance>& instance_proxy<InstanceInfo>::instances_mapped() const noexcept
{
    return _instance_descriptions_src[_current_instance_index];
}
template<typename InstanceInfo>
size_t instance_proxy<InstanceInfo>::instance_buffer_index() const noexcept
{
    return _current_instance_index;
}
}    // namespace vulkan
}    // namespace v1
}    // namespace gfx