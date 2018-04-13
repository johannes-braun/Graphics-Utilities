#pragma once

#include "version.hpp"
#include "structure.hpp"
#include "array_view.hpp"
#include "physical_device.hpp"

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace gfx::vk
{
    struct app_info : structure<VK_STRUCTURE_TYPE_APPLICATION_INFO> {
        constexpr app_info(const char* app_name, version app_version, const char* engine_name, version engine_version, version api_version ={ 1, 0 })
            : app_name(app_name), app_version(app_version), engine_name(engine_name), engine_version(engine_version), api_version(api_version) {}

    public:
        const char* app_name         = nullptr;
        version     app_version      = 0;
        const char* engine_name      = nullptr;
        version     engine_version   = 0;
        version     api_version      = 0;
    };

    class instance : public std::enable_shared_from_this<instance>
    {
    public:
        instance(app_info info, array_view<const char*> layers, array_view<const char*> extensions);
        ~instance();
        instance(const instance&) = delete;
        instance& operator=(const instance&) = delete;
        instance(instance&&) = default;
        instance& operator=(instance&&) = default;

        void* proc_address(const char* name) const noexcept;
        std::vector<std::shared_ptr<physical_device>> get_physical_devices();
        operator VkInstance() const noexcept;

    private:
        VkInstance _instance;
    };
}