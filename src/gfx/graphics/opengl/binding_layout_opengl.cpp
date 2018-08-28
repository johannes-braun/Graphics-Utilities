#include "binding_layout_opengl.hpp"
#include <algorithm>

namespace gfx {
inline namespace v1 {
namespace opengl {
void binding_layout_implementation::push(binding_type type, u32 array_size)
{
    const u32 t = u32(type);
	_binding_counts.resize(std::max(u32(_binding_counts.size()), t + 1));

    const u32 base = [&] {
		if (_binding_counts[t].empty())
			return 0u;
		return _binding_counts[t].back().begin + _binding_counts[t].back().count;
	}();

	_items.push_back(static_cast<u32>(_binding_counts[t].size()));
	range& r = _binding_counts[t].emplace_back();
	r.count = array_size;
	r.begin = base;
}

handle binding_layout_implementation::api_handle()
{
    return {};
}

const std::vector<std::vector<binding_layout_implementation::range>>& binding_layout_implementation::bindings() const noexcept
{
    return _binding_counts;
}

const std::vector<u32>& binding_layout_implementation::items() const noexcept
{
	return _items;
}
}    // namespace opengl
}    // namespace v1
}    // namespace gfx