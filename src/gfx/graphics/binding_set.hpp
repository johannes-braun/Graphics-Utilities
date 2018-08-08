#pragma once

#include "../api.hpp"
#include "implementation.hpp"
#include "binding_layout.hpp"
#include "device_buffer.hpp"
#include "image_view.hpp"
#include "sampler.hpp"
#include <memory>

namespace gfx {
inline namespace v1 {

namespace detail {
class binding_set_implementation
{
public:
    virtual ~binding_set_implementation() = default;
    static std::unique_ptr<binding_set_implementation> make();

	virtual void initialize(const binding_layout& layout) = 0;
	virtual void bind(u32 binding, u32 arr_element, binding_type type, std::any obj) = 0;
    virtual handle api_handle() = 0;
};
}    // namespace detail

class binding_set : public impl::implements<detail::binding_set_implementation>
{
public:
	binding_set(const binding_layout& layout);

	template<typename T>
	void bind(u32 binding, const device_buffer<T>& buffer);
	template<typename T>
	void bind(u32 binding, const host_buffer<T>& buffer);
	void bind(u32 binding, const image_view& view, const sampler& sampler);
	void bind(u32 binding, const image_view& view);
	template<typename T>
	void bind(u32 binding, u32 array_element, const device_buffer<T>& buffer);
	template<typename T>
	void bind(u32 binding, u32 array_element, const host_buffer<T>& buffer);
	void bind(u32 binding, u32 array_element, const image_view& view, const sampler& sampler);
	void bind(u32 binding, u32 array_element, const image_view& view);

private:
	const binding_layout* _layout;
};

template<typename T>
void binding_set::bind(u32 binding, const device_buffer<T>& buffer)
{
	assert(_layout->types()[binding] == binding_type::uniform_buffer || _layout->types()[binding] == binding_type::storage_buffer);
	implementation()->bind(binding, 0, _layout->types()[binding], &buffer.implementation());
}

template<typename T>
void binding_set::bind(u32 binding, const host_buffer<T>& buffer)
{
	assert(_layout->types()[binding] == binding_type::uniform_buffer || _layout->types()[binding] == binding_type::storage_buffer);
	implementation()->bind(binding, 0, _layout->types()[binding], &buffer.implementation());
}

template<typename T>
void binding_set::bind(u32 binding, u32 array_element, const device_buffer<T>& buffer)
{
	assert(_layout->types()[binding] == binding_type::uniform_buffer || _layout->types()[binding] == binding_type::storage_buffer);
	implementation()->bind(binding, array_element, _layout->types()[binding], &buffer.implementation());
}

template<typename T>
void binding_set::bind(u32 binding, u32 array_element, const host_buffer<T>& buffer)
{
	assert(_layout->types()[binding] == binding_type::uniform_buffer || _layout->types()[binding] == binding_type::storage_buffer);
	implementation()->bind(binding, array_element, _layout->types()[binding], &buffer.implementation());
}

}    // namespace v1
}    // namespace gfx