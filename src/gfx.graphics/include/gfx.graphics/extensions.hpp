#pragma once

#include <vulkan/vulkan.hpp>

namespace gfx {
inline namespace v1 {
struct extension_dispatch
{
    void load(vk::Instance instance);
    void load(vk::Device device);

    PFN_vkCreateDebugUtilsMessengerEXT  vkCreateDebugUtilsMessengerEXT  = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;
    PFN_vkCreateDebugReportCallbackEXT  vkCreateDebugReportCallbackEXT  = nullptr;
    PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = nullptr;
    PFN_vkCreateSwapchainKHR            vkCreateSwapchainKHR            = nullptr;
    PFN_vkDestroySwapchainKHR           vkDestroySwapchainKHR           = nullptr;
};
}    // namespace v1
}    // namespace gfx