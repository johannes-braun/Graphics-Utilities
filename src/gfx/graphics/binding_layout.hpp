#pragma once

#include "../type.hpp"
#include "implementation.hpp"
#include <vector>
#include <memory>

namespace gfx {
inline namespace v1 {
enum class binding_type
{
    storage_buffer = 0,
    uniform_buffer,
    storage_image,
    sampled_image
};

namespace detail {
class binding_layout_implementation
{
public:
    virtual ~binding_layout_implementation() = default;
    static std::unique_ptr<binding_layout_implementation> make();

	virtual void   initialize(bool dynamic) = 0;
    virtual void   push(binding_type type, u32 array_size) = 0;
    virtual handle api_handle()                            = 0;
};
}    // namespace detail

class binding_layout : public impl::implements<detail::binding_layout_implementation>
{
public:
	binding_layout(bool dynamic = false) : _dynamic(dynamic) { implementation()->initialize(_dynamic); }
    binding_layout& push(binding_type type, u32 array_size = 1);
	const std::vector<binding_type>& types() const;
	bool dynamic() const { return _dynamic; }

private:
	bool _dynamic;
	std::vector<binding_type> _types;
};
}    // namespace v1
}    // namespace gfx
