#pragma once

#include <vulkan/vulkan.hpp>
#include <optional>
#include <gfx.core/types.hpp>

namespace gfx
{
	inline namespace v1
	{
		template<typename T>
		using cref_array_view = vk::ArrayProxy<const std::reference_wrapper<const T>>;
		template<typename T>
		using opt_ref = std::optional<std::reference_wrapper<T>>;
	}
}