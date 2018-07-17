#pragma once

#include "device_buffer.hpp"
#include "device_image.hpp"
#include "image_view.hpp"
#include "sampler.hpp"
#include <any>
#include <vector>

namespace gfx
{
enum class descriptor_type
{
    uniform_buffer,
    storage_buffer,
    sampled_texture,
    image
};

namespace detail
{
class descriptor_set_implementation
{
public:
    virtual ~descriptor_set_implementation()                                                               = default;
    virtual void     update(descriptor_type type, uint32_t binding, std::initializer_list<std::any> infos) = 0;
    virtual std::any api_handle()                                                                          = 0;
};
std::unique_ptr<descriptor_set_implementation> make_descriptor_set_implementation();
}

class commands;
class descriptor_set
{
public:
    friend class commands;
    descriptor_set();

    template<typename T>
    void set(descriptor_type type, uint32_t binding, const device_buffer<T>& buffer);
    template<typename T>
    void set(descriptor_type type, uint32_t binding, const host_buffer<T>& buffer);

    void set(descriptor_type type, uint32_t binding, const image_view& view, const sampler& sampler);

    size_t type_count() const noexcept { return _bindings.size(); }
    const std::vector<std::pair<uint32_t, std::any>>& bindings(descriptor_type type) const noexcept { return _bindings[static_cast<uint32_t>(type)]; }

private:
    std::any& get_at_or_create(descriptor_type t, uint32_t binding);

    std::vector<std::vector<std::pair<uint32_t, std::any>>> _bindings;
    std::unique_ptr<detail::descriptor_set_implementation>  _implementation;
};
}

#include "descriptor.inl"