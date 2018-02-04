#pragma once
#include <vulkan/vulkan.hpp>
#include <functional>
#include <map>


namespace vkn
{
    
    struct QueueFilter
    {
        using FilterProps = std::function<bool(const vk::QueueFamilyProperties&)>;
        using FilterFamily = std::function<bool(int32_t)>;
        using FilterBoth = std::function<bool(int32_t, const vk::QueueFamilyProperties&)>;

        QueueFilter(float priority, vk::QueueFlags flags);
        QueueFilter(float priority, int32_t family);
        QueueFilter(float priority, FilterProps filter);
        QueueFilter(float priority, FilterFamily filter);
        QueueFilter(float priority, FilterBoth filter);

        float priority = 1.f;
        FilterBoth filter;
    };

	class QueueCreator
	{
	public:
		QueueCreator(vk::PhysicalDevice physical_device, vk::ArrayProxy<const QueueFilter> filters);

		const std::vector<uint32_t>& families() const;
		const std::vector<uint32_t>& uniqueFamilies() const;
		const std::vector<vk::DeviceQueueCreateInfo>& createInfos() const;

	private:
		std::map<uint32_t, std::tuple<uint32_t, std::vector<float>>> m_family_filter;
		std::vector<uint32_t> m_families;
		std::vector<uint32_t> m_unique_families;
		std::vector<vk::DeviceQueueCreateInfo> m_queue_infos;
	};
}
