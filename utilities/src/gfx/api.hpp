#pragma once

#include <GLFW/glfw3.h>
#include "interface/window_handler.hpp"
#include "interface/imgui_handler.hpp"
#include <unordered_map>
#include <functional>

namespace gfx
{
    struct api
    {
        constexpr static const auto name        = "none";
        constexpr static const auto api_type    = GLFW_NO_API;
        
        using window_handler_type   = window_handler;
        using imgui_handler_type    = imgui_handler;
    };

    class api_manager
    {
    public:
        using window_handler_create_fun = std::function<std::unique_ptr<window_handler>(window& w)>;
        using imgui_handler_create_fun  = std::function<std::unique_ptr<imgui_handler >(imgui & i)>;
        struct api_info
        {
            size_t api_type                             = GLFW_NO_API;
            window_handler_create_fun   create_window   = [](window&) -> std::unique_ptr<window_handler> { return nullptr; };
            imgui_handler_create_fun    create_imgui    = [](imgui&)  -> std::unique_ptr<imgui_handler> { return nullptr; };
        };
        
        template<typename T, typename = std::void_t<std::enable_if_t<std::is_base_of_v<api, T>>, decltype(T::name), decltype(T::api_type), typename T::window_handler_type, typename T::imgui_handler_type>>
        static void register_api()
        {
            api_info info;
            info.api_type = T::api_type;
            info.create_window  = [](window& w) { return std::make_unique<typename T::window_handler_type>(w); };
            info.create_imgui   = [](imgui& w) { return std::make_unique<typename T::imgui_handler_type >(w); };
            apis()[T::name] = info;
        }

        static const api_info& info(const std::string& name)
        {
            return apis()[name];
        }

    private:
        static std::unordered_map<std::string, api_info>& apis() {
            static std::unordered_map<std::string, api_info> a;
            return a;
        }
    };

    template<typename T, typename = std::void_t<std::enable_if_t<std::is_base_of_v<api, T>>, decltype(T::name), decltype(T::api_type), typename T::window_handler_type, typename T::imgui_handler_type>>
    struct enable_api
    {
        enable_api() { api_manager::register_api<T>(); }
        enable_api(const enable_api&) = delete;
        enable_api(enable_api&&) = delete;
        enable_api& operator=(const enable_api&) = delete;
        enable_api& operator=(enable_api&&) = delete;
    };


    namespace apis
    {
        struct opengl : api
        {
            constexpr static const auto name        = "opengl";
            constexpr static const auto api_type    = GLFW_OPENGL_API;

            using window_handler_type   = window_handler_opengl;
            using imgui_handler_type    = imgui_handler_opengl;
        };
        static enable_api<opengl> opengl_enabled;

        struct vulkan : api
        {
            constexpr static const auto name        = "vulkan";
            constexpr static const auto api_type    = GLFW_NO_API;

            using window_handler_type   = window_handler_vulkan;
            using imgui_handler_type    = imgui_handler_vulkan;
        };
        static enable_api<vulkan> vulkan_enabled;
    }
}