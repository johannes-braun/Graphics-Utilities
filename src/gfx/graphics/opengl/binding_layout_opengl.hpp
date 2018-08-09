#pragma once

#include "../binding_layout.hpp"
#include <gfx/api.hpp>
#include <functional>

namespace gfx {
inline namespace v1 {
namespace opengl {

class binding_layout_implementation : public detail::binding_layout_implementation
{
public:
	struct range
	{
		u32 begin;
		u32 count;
	};

	void   initialize(bool dynamic) override { _dynamic=dynamic; }
    void   push(binding_type type, u32 array_size) override;
    handle api_handle() override;

	const std::vector<std::vector<range>>& bindings() const noexcept;
	const std::vector<u32>& items() const noexcept;

private:
	bool _dynamic;
	std::vector<u32> _items;
	std::vector<std::vector<range>> _binding_counts;
};

}    // namespace opengl
}    // namespace v1
}    // namespace gfx