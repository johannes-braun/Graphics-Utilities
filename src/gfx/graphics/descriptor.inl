#pragma once

namespace gfx {
inline namespace v1 {
template<typename T>
void descriptor_set::set(descriptor_type type, uint32_t binding, const device_buffer<T>& buffer)
{
    get_at_or_create(type, binding) = buffer.api_handle();
    implementation()->update(type, binding, {buffer.api_handle()});
}

template<typename T>
void descriptor_set::set(descriptor_type type, uint32_t binding, const host_buffer<T>& buffer)
{
    get_at_or_create(type, binding) = buffer.api_handle();
    implementation()->update(type, binding, {buffer.api_handle()});
}
}    // namespace v1
}    // namespace gfx