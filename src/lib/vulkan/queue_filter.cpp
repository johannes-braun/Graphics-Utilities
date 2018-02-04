#include "queue_filter.hpp"

namespace vkn
{
	QueueCreator::QueueCreator(vk::PhysicalDevice physical_device, vk::ArrayProxy<const QueueFilter> filters)
	{
		m_families.resize(filters.size(), -1);

		auto family_properties = physical_device.getQueueFamilyProperties();
		for (int32_t family = 0; family < family_properties.size(); ++family)
		{
			const auto properties = family_properties[family];
			for (size_t idx = 0; idx < filters.size(); ++idx)
			{
				auto&& filter = filters.data()[idx];
				if (m_families[idx] == -1 && filter.filter(family, properties))
				{
					auto&& tuple = m_family_filter[m_families[idx] = family];
					++std::get<0>(tuple);
					std::get<1>(tuple).push_back(filter.priority);
				}
			}
		}

		for (const auto& family : m_family_filter)
		{
			m_queue_infos.push_back(vk::DeviceQueueCreateInfo({}, family.first, std::get<0>(family.second),
			                                                  std::get<1>(family.second).data()));
			m_unique_families.push_back(family.first);
		}
	}

	const std::vector<uint32_t>& QueueCreator::families() const
	{
		return m_families;
	}

	const std::vector<uint32_t>& QueueCreator::uniqueFamilies() const
	{
		return m_unique_families;
	}

	const std::vector<vk::DeviceQueueCreateInfo>& QueueCreator::createInfos() const
	{
		return m_queue_infos;
	}

    QueueFilter::QueueFilter(const float priority, const vk::QueueFlags flags)
        : priority(priority), filter([flags](auto, const auto& props) { return static_cast<bool>(props.queueFlags & vk::QueueFlagBits::eGraphics); })
    {

    }

    QueueFilter::QueueFilter(const float priority, const int32_t family)
        : priority(priority), filter([family](auto fm, const auto&) { return fm == family; })
    {
    }

    QueueFilter::QueueFilter(const float priority, const FilterProps filter)
        : priority(priority), filter([filter](auto, const auto& props) { return filter(props); })
    {
    }

    QueueFilter::QueueFilter(const float priority, const FilterFamily filter)
        : priority(priority), filter([filter](auto fm, const auto&) { return filter(fm); })
    {
    }

    QueueFilter::QueueFilter(const float priority, const FilterBoth filter)
        : priority(priority), filter(filter)
    {
    }
}
