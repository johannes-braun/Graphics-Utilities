#define GLFW_EXPOSE_NATIVE_WIN32

#include <cinttypes>
#include <vector>
#if _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <vulkan/vulkan.hpp>

inline PFN_vkCreateDebugUtilsMessengerEXT impl_vkCreateDebugUtilsMessengerEXT;
VkResult vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                        const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
{
    return impl_vkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}
inline PFN_vkDestroyDebugUtilsMessengerEXT impl_vkDestroyDebugUtilsMessengerEXT;
void vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT pMessenger, const VkAllocationCallbacks* pAllocator)
{
    impl_vkDestroyDebugUtilsMessengerEXT(instance, pMessenger, pAllocator);
}

namespace gfx {
inline namespace v1 {
using u32 = uint32_t;
using u16 = uint16_t;

struct version_t
{
    constexpr version_t(u16 major, u16 minor, u16 patch) : major(major), minor(minor), patch(patch) {}
    constexpr version_t() : version_t(0, 0, 0) {}
    constexpr operator u32&() { return *reinterpret_cast<u32*>(this); }
    constexpr operator const u32&() const { return *reinterpret_cast<const u32*>(this); }

    u16 major : 10;
    u16 minor : 10;
    u16 patch : 12;
};

enum class api
{
    vulkan
};

namespace detail {
constexpr version_t        engine_version = version_t(1, 0, 0);
constexpr std::string_view engine_name    = "Graphics Utilities";
}    // namespace detail

class instance
{
public:
    instance(std::string_view app_name, version_t app_version, bool debug, bool surface_support);
    ~instance();

	vk::Instance inst() const noexcept { return _instance.get(); }

private:
    vk::UniqueInstance               _instance;
    vk::UniqueDebugUtilsMessengerEXT _debug_messenger;
};

instance::instance(std::string_view app_name, version_t app_version, bool debug, bool surface_support)
{
    vk::ApplicationInfo app_info;
    app_info.apiVersion         = version_t(1, 1, 0);
    app_info.applicationVersion = app_version;
    app_info.engineVersion      = detail::engine_version;
    app_info.pEngineName        = detail::engine_name.data();
    app_info.pApplicationName   = app_name.data();

    std::vector<const char*> extensions;
    if (surface_support)
    {
        extensions.push_back("VK_KHR_surface");
        extensions.push_back("VK_KHR_win32_surface");
    }
    if (debug) extensions.push_back("VK_EXT_debug_utils");

    std::vector<const char*> layers;
    if (debug) layers.push_back("VK_LAYER_LUNARG_standard_validation");

    vk::InstanceCreateInfo instance_info({}, &app_info, static_cast<u32>(layers.size()), layers.data(), static_cast<u32>(extensions.size()),
                                         extensions.data());
    _instance = vk::createInstanceUnique(instance_info);

    impl_vkCreateDebugUtilsMessengerEXT =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(_instance->getProcAddr("vkCreateDebugUtilsMessengerEXT"));
    impl_vkDestroyDebugUtilsMessengerEXT =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(_instance->getProcAddr("vkDestroyDebugUtilsMessengerEXT"));

    if (debug)
    {
        using sev = vk::DebugUtilsMessageSeverityFlagBitsEXT;
        vk::DebugUtilsMessengerCreateInfoEXT debug_messenger_info(
            {}, sev::eError | sev::eWarning | sev::eInfo, vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral,
            [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
               const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) -> VkBool32 {
                switch (sev(messageSeverity))
                {
                case sev::eError: break;
                case sev::eWarning: break;
                case sev::eInfo: break;
                }
                return false;
            });
        _debug_messenger = _instance->createDebugUtilsMessengerEXTUnique(debug_messenger_info);
    }
}

instance::~instance() {}
}    // namespace v1
}    // namespace gfx

int main()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	auto* w = (glfwInit(), glfwCreateWindow(1280, 720, "aois", nullptr, nullptr));
	HWND win = glfwGetWin32Window(w);

	gfx::instance my_app("Application", gfx::version_t(1, 0, 0), true, true);

	vk::Win32SurfaceCreateInfoKHR ci;
	ci.hwnd = win;
	auto us = my_app.inst().createWin32SurfaceKHRUnique(ci);
}