#include "../instance.hpp"

namespace gfx::vk
{
    instance::instance(app_info info, array_view<const char*> layers, array_view<const char*> extensions) {
        VkInstanceCreateInfo create_info;
        create_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pNext                   = nullptr;
        create_info.flags                   = 0;
        create_info.enabledExtensionCount   = uint32_t(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();
        create_info.enabledLayerCount       = uint32_t(layers.size());
        create_info.ppEnabledLayerNames     = layers.data();
        create_info.pApplicationInfo        = reinterpret_cast<VkApplicationInfo*>(&info);
        VkResult r = vkCreateInstance(&create_info, nullptr, &_instance);
    }

    instance::~instance()
    { 
        vkDestroyInstance(_instance, nullptr); 
    }

    instance::operator VkInstance() const noexcept 
    {
        return _instance; 
    }

    void* instance::proc_address(const char* name) const noexcept 
    {
        return vkGetInstanceProcAddr(_instance, name); 
    }

    std::vector<std::shared_ptr<physical_device>> instance::get_physical_devices()
    {
        uint32_t count = 0; vkEnumeratePhysicalDevices(_instance, &count, nullptr);
        std::vector<VkPhysicalDevice> devices(count);
        vkEnumeratePhysicalDevices(_instance, &count, devices.data());

        auto wptr = std::shared_ptr<instance>(this, [](instance*) {});
        std::vector<std::shared_ptr<physical_device>> physical_devices;
        physical_devices.reserve(count);
        for (auto&& device : devices)
            physical_devices.emplace_back(new physical_device(shared_from_this(), device));
        return physical_devices;
    }
}