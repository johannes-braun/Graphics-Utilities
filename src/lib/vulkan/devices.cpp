#include "devices.hpp"
#include "queue_filter.hpp"

namespace vkn
{
	vk::PhysicalDevice devices::findPhysicalIf(const vk::Instance instance, const Predicate suitable_if)
	{
		switch (auto available = instance.enumeratePhysicalDevices(); available.size())
		{
		case 0:
			throw std::runtime_error("Cannot fetch suitable Physical Device");
		case 1:
			return available[0];
		default:
			if (const auto it = std::find_if(available.begin(), available.end(), suitable_if); it != available.end())
				return *it;
            throw std::runtime_error("Cannot fetch suitable Physical Device");
		}
	}

	vk::PhysicalDevice devices::findPhysical(const vk::Instance instance)
	{
		return findPhysicalIf(instance, [](auto){ return true; });
	}
}
