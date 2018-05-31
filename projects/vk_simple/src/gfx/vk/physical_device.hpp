#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace vk
{
    class instance;
    class device;
    class surface;
    class physical_device
    {
    public:
        friend instance;
        friend device;
        friend surface;

        physical_device(const physical_device&) = delete;
        physical_device& operator=(const physical_device&) = delete;
        physical_device(physical_device&&) = default;
        physical_device& operator=(physical_device&&) = default;

        VkPhysicalDeviceFeatures get_features() const noexcept
        {
            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(_physical_device, &features);
            return features;
        }

        std::vector<VkQueueFamilyProperties> get_queue_family_properties() const noexcept
        {
            uint32_t count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &count, nullptr);
            std::vector<VkQueueFamilyProperties> props(count);
            vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &count, props.data());
            return props;

        }

        operator VkPhysicalDevice() const noexcept
        {
            return _physical_device;
        }

    private:
        physical_device(const std::shared_ptr<instance>& instance, VkPhysicalDevice pd) : _instance(instance), _physical_device(pd) {}

        std::shared_ptr<instance> _instance;
        VkPhysicalDevice _physical_device = nullptr;
    };
}