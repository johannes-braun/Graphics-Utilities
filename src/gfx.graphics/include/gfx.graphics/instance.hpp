#pragma once

#include "extensions.hpp"
#include <gfx.core/version.hpp>
#include <vulkan/vulkan.hpp>

namespace gfx {
inline namespace v1 {
class instance
{
public:
    explicit instance(std::string_view app_name, version_t app_version, bool debug, bool surface_support,
                      vk::ArrayProxy<char const* const> additional_extensions = {});
    ~instance() = default;

    instance(instance const& other);
    instance(instance&& other) noexcept;
    instance& operator=(instance const& other);
    instance& operator=(instance&& other) noexcept;

    [[nodiscard]] auto get_instance() const noexcept -> vk::Instance const&;
    [[nodiscard]] auto get_dispatcher() const noexcept -> extension_dispatch const&;
    [[nodiscard]] auto is_debug() const noexcept -> bool;
    [[nodiscard]] auto is_surface_supported() const noexcept -> bool;

private:
    void initialize(std::string_view app_name, version_t app_version, bool debug, bool surface_support,
                    vk::ArrayProxy<char const* const> additional_extensions);

    std::string                                                      _app_name;
    version_t                                                        _app_version;
    vk::UniqueInstance                                               _instance;
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, extension_dispatch> _debug_messenger;
    extension_dispatch                                               _dispatcher;
    vk::UniqueHandle<vk::DebugReportCallbackEXT, extension_dispatch> _debug_callback;
    std::vector<char const*>                                         _extensions;

    struct
    {
        bool debug           = false;
        bool surface_support = false;
    } _capabilities;
};

}    // namespace v1
}    // namespace gfx