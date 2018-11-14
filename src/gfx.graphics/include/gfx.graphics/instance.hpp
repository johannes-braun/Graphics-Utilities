#pragma once

#include "extensions.hpp"
#include <gfx.core/version.hpp>
#include <vulkan/vulkan.hpp>

namespace gfx {
inline namespace v1 {
class instance
{
public:
	explicit instance(std::string_view app_name, version_t app_version, bool debug, bool surface_support, vk::ArrayProxy<const char* const> additional_extensions = {});
    ~instance() = default;

	instance(const instance& other);
	instance(instance&& other) noexcept;
	instance& operator=(const instance& other);
	instance& operator=(instance&& other) noexcept;

    const vk::Instance&       get_instance() const noexcept;
    const extension_dispatch& get_dispatcher() const noexcept;

    bool is_debug() const noexcept;
    bool is_surface_supported() const noexcept;

private:
	void initialize(std::string_view app_name, version_t app_version, bool debug, bool surface_support, vk::ArrayProxy<const char* const> additional_extensions);

    std::string                                                      _app_name;
    version_t                                                        _app_version;
    vk::UniqueInstance                                               _instance;
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, extension_dispatch> _debug_messenger;
    extension_dispatch                                               _dispatcher;
    vk::UniqueHandle<vk::DebugReportCallbackEXT, extension_dispatch> _debug_callback;
	std::vector<const char*> _extensions;

    struct
    {
        bool debug           = false;
        bool surface_support = false;
    } _capabilities;
};

}    // namespace v1
}    // namespace gfx