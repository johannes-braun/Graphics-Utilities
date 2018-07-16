#pragma once

namespace gfx
{
template<typename T>
void descriptor_set::set(descriptor_type type, uint32_t binding, const device_buffer<T>& buffer)
{
    get_at_or_create(type, binding) = buffer.api_handle();
    _implementation->update(type, binding, {buffer.api_handle()});
}

template<typename T>
void descriptor_set::set(descriptor_type type, uint32_t binding, const host_buffer<T>& buffer)
{
    get_at_or_create(type, binding) = buffer.api_handle();
    _implementation->update(type, binding, {buffer.api_handle()});
}
}