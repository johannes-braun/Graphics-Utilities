#pragma once

#include <vulkan/vulkan.hpp>
#include <functional>

namespace vkn::devices
{
	using Predicate = std::function<bool(vk::PhysicalDevice device)>;
	vk::PhysicalDevice findPhysicalIf(const vk::Instance instance, const Predicate suitable_if);
	vk::PhysicalDevice findPhysical(const vk::Instance instance);
}