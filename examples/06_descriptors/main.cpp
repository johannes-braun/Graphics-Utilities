#define GLFW_EXPOSE_NATIVE_WIN32

#include <cinttypes>
#include <vector>
#if _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <gsl/gsl>
#include <optional>
#include <unordered_map>
#include <vulkan/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include <QApplication>
#include <QWidget>
#include <QWindow>
#include <gfx/log.hpp>
#include <unordered_set>

struct ext_dispatcher
{
    void load(vk::Instance instance)
    {
#define load_fun(Name) impl_##Name = reinterpret_cast<PFN_##Name>(instance.getProcAddr(#Name))

        load_fun(vkCreateDebugUtilsMessengerEXT);
        load_fun(vkDestroyDebugUtilsMessengerEXT);

#undef load_fun
    }

    void load(vk::Device device)
    {
#define load_fun(Name) impl_##Name = reinterpret_cast<PFN_##Name>(device.getProcAddr(#Name))

        load_fun(vkCreateSwapchainKHR);
        load_fun(vkDestroySwapchainKHR);

#undef load_fun
    }

    VkResult vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                            const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger) const noexcept
    {
        return impl_vkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
    }

    void vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT pMessenger,
                                         const VkAllocationCallbacks* pAllocator) const noexcept
    {
        impl_vkDestroyDebugUtilsMessengerEXT(instance, pMessenger, pAllocator);
    }

    VkResult vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator,
                                  VkSwapchainKHR* pSwapchain) const noexcept
    {
        return impl_vkCreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
    }

    void vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator) const noexcept
    {
        impl_vkDestroySwapchainKHR(device, swapchain, pAllocator);
    }

private:
    PFN_vkCreateDebugUtilsMessengerEXT  impl_vkCreateDebugUtilsMessengerEXT  = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT impl_vkDestroyDebugUtilsMessengerEXT = nullptr;
    PFN_vkCreateSwapchainKHR            impl_vkCreateSwapchainKHR            = nullptr;
    PFN_vkDestroySwapchainKHR           impl_vkDestroySwapchainKHR           = nullptr;
};

namespace gfx {
inline namespace v1 {
using u32 = uint32_t;
using u16 = uint16_t;

struct version_t
{
    constexpr version_t(u16 major, u16 minor, u16 patch) : major(major), minor(minor), patch(patch) {}
    constexpr version_t() : version_t(0, 0, 0) {}
    constexpr operator u32() const { return *reinterpret_cast<const u32*>(this); }

    u32 patch : 12, minor : 10, major : 10;
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
    ~instance() = default;

    vk::Instance          inst() const noexcept { return _instance.get(); }
    const ext_dispatcher& dispatcher() const noexcept { return _dispatcher; }

    bool is_debug() const noexcept { return _capabilities.debug; }
    bool is_surface_supported() const noexcept { return _capabilities.surface_support; }

private:
    vk::UniqueInstance                                           _instance;
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, ext_dispatcher> _debug_messenger;
    ext_dispatcher                                               _dispatcher;

    struct
    {
        bool debug           = false;
        bool surface_support = false;
    } _capabilities;
};

instance::instance(std::string_view app_name, version_t app_version, bool debug, bool surface_support)
      : _capabilities{debug, surface_support}
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

    const vk::InstanceCreateInfo instance_info({}, &app_info, static_cast<u32>(layers.size()), layers.data(),
                                               static_cast<u32>(extensions.size()), extensions.data());
    _instance = vk::createInstanceUnique(instance_info);
    _dispatcher.load(_instance.get());

    if (debug)
    {
        using sev = vk::DebugUtilsMessageSeverityFlagBitsEXT;
        const vk::DebugUtilsMessengerCreateInfoEXT debug_messenger_info(
            {}, sev::eError | sev::eWarning | sev::eInfo, vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral,
            [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
               const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) -> VkBool32 {
                switch (sev(messageSeverity))
                {
                case sev::eError: gfx::elog << pCallbackData->pMessage; break;
                case sev::eWarning: gfx::wlog << pCallbackData->pMessage; break;
                case sev::eInfo: gfx::ilog << pCallbackData->pMessage; break;
                case sev::eVerbose: gfx::dlog << pCallbackData->pMessage; break;
                }
                return false;
            });
        _debug_messenger = _instance->createDebugUtilsMessengerEXTUnique(debug_messenger_info, nullptr, _dispatcher);
    }
}

class surface
{
public:
    surface(instance& i, GLFWwindow* glfw_window)
    {
        _surface = i.inst().createWin32SurfaceKHRUnique({{}, nullptr, glfwGetWin32Window(glfw_window)});
    }

    surface(instance& i, QWidget* qt_window)
    {
		auto wnd = qt_window->winId();
        _surface = i.inst().createWin32SurfaceKHRUnique({{}, nullptr, HWND(wnd)});
    }

    vk::SurfaceKHR surf() const noexcept { return _surface.get(); }

private:
    vk::UniqueSurfaceKHR _surface;
};

class queue
{
public:
    queue() = delete;

    void submit(vk::ArrayProxy<std::reference_wrapper<const std::commands>> cmds)
    {
        std::vector<vk::CommandBuffer> submits(cmds.size());
        for(size_t i = 0; i < submits.size(); ++i)
            submits[i] = cmds[i].get().cmd();
        
        vk::SubmitInfo submit;
        submit.commandBufferCount = u32(submits.size());
        submit.pCommandBuffers = submits.data();
        // Semaphores...
        
        _queue.submit(submit, nullptr);
    }
    
    vk::Queue get() const noexcept { return _queue; }
    
private:
    vk::Queue _queue;
};

class commands
{
public:
    vk::CommandBuffer cmd() const noexcept { return _buf.get(); }
    
private:
    vk::UniqueCommandBuffer _buf;
}

enum class queue_type
{
    graphics,
    compute,
    transfer,
    present
};

enum class device_target
{
    igpu,
    gpu,
    cpu,
    vgpu
};

class device
{
public:
    device(instance& i, device_target target, vk::ArrayProxy<const float> graphics_priorities,
           vk::ArrayProxy<const float> compute_priorities, std::optional<std::reference_wrapper<surface>> surface = std::nullopt)
    {
        const auto             gpus        = i.inst().enumeratePhysicalDevices();
        const vk::PhysicalDeviceType target_type = [target] {
            using dt  = device_target;
            using vdt = vk::PhysicalDeviceType;
            switch (target)
            {
            case dt::cpu: return vdt::eCpu;
            case dt::gpu: return vdt::eDiscreteGpu;
            case dt::igpu: return vdt::eIntegratedGpu;
            case dt::vgpu: return vdt::eVirtualGpu;
            }
        }();
		vk::PhysicalDevice dgpu;
		vk::PhysicalDevice igpu;
		vk::PhysicalDevice cpu;
        for (const auto& gpu : gpus)
        {
            if (gpu.getProperties().deviceType == target_type)
            {
                _gpu = gpu;
                break;
            }
			if (gpu.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
				dgpu = gpu;
			else if (gpu.getProperties().deviceType == vk::PhysicalDeviceType::eCpu)
				cpu = gpu;
			else if (gpu.getProperties().deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
				igpu = gpu;
        }
        if(!_gpu)
        {
			if (dgpu)
				_gpu = dgpu;
			else if (igpu)
				_gpu = igpu;
			else if (cpu)
				_gpu = cpu;
        }

        const bool enable_present                   = surface && i.is_surface_supported();
        const auto queue_properties                 = _gpu.getQueueFamilyProperties();
        const auto [fgraphics, fcompute, ftransfer] = dedicated_families(queue_properties);
        const auto fpresent                         = enable_present ? presentation_family(i, *surface, queue_properties) : 0;
        std::unordered_map<u32, vk::DeviceQueueCreateInfo> queue_create_infos;
        std::unordered_map<u32, std::vector<float>>        priorities;

        priorities[fgraphics].insert(priorities[fgraphics].end(), graphics_priorities.begin(), graphics_priorities.end());
        priorities[fcompute].insert(priorities[fcompute].end(), compute_priorities.begin(), compute_priorities.end());
        priorities[ftransfer].emplace_back(1.f);
        if (enable_present) priorities[fpresent].emplace_back(1.f);

        auto& graphics_create_info            = queue_create_infos[fgraphics];
        graphics_create_info.queueFamilyIndex = fgraphics;
        graphics_create_info.queueCount       = priorities[fgraphics].size();
        graphics_create_info.pQueuePriorities = priorities[fgraphics].data();
        auto& compute_create_info             = queue_create_infos[fcompute];
        compute_create_info.queueFamilyIndex  = fcompute;
        compute_create_info.queueCount        = priorities[fcompute].size();
        compute_create_info.pQueuePriorities  = priorities[fcompute].data();
        auto& transfer_create_info            = queue_create_infos[ftransfer];
        transfer_create_info.queueFamilyIndex = ftransfer;
        transfer_create_info.queueCount       = priorities[ftransfer].size();
        transfer_create_info.pQueuePriorities = priorities[ftransfer].data();
        if (enable_present)
        {
            auto& present_create_info            = queue_create_infos[fpresent];
            present_create_info.queueFamilyIndex = fpresent;
            present_create_info.queueCount       = priorities[fpresent].size();
            present_create_info.pQueuePriorities = priorities[fpresent].data();
        }
        std::vector<vk::DeviceQueueCreateInfo> queue_infos(queue_create_infos.size());
        {
            int index = 0;
            for (auto& p : queue_create_infos) queue_infos[index++] = p.second;
        }

        vk::PhysicalDeviceFeatures features = _gpu.getFeatures();
        vk::DeviceCreateInfo       device_create_info;
        device_create_info.pEnabledFeatures = &features;

        std::vector<const char*> extensions{VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME, VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
                                            VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME};
        if (i.is_surface_supported()) extensions.push_back("VK_KHR_swapchain");
        std::vector<const char*> layers;
        if (i.is_debug()) layers.push_back("VK_LAYER_LUNARG_standard_validation");

        device_create_info.enabledExtensionCount   = extensions.size();
        device_create_info.ppEnabledExtensionNames = extensions.data();
        device_create_info.enabledLayerCount       = layers.size();
        device_create_info.ppEnabledLayerNames     = layers.data();
        device_create_info.queueCreateInfoCount    = queue_infos.size();
        device_create_info.pQueueCreateInfos       = queue_infos.data();
        _device                                    = _gpu.createDeviceUnique(device_create_info);
        _dispatcher.load(_device.get());
        std::unordered_map<u32, u32> queue_counter;

        for (size_t index = 0; index < graphics_priorities.size(); ++index)
        {
            _queues[u32(queue_type::graphics)].push_back(_device->getQueue(fgraphics, queue_counter[fgraphics]++));
            _command_pools[u32(queue_type::graphics)].push_back(_device->createCommandPool({vk::CommandPoolCreateFlagBits::eResetCommandBuffer, fgraphics}));
        }

        for (size_t index = 0; index < compute_priorities.size(); ++index)
        {
            _queues[u32(queue_type::compute)].push_back(_device->getQueue(fcompute, queue_counter[fcompute]++));
            _command_pools[u32(queue_type::compute)].push_back(_device->createCommandPool({vk::CommandPoolCreateFlagBits::eResetCommandBuffer, fcompute}));
        }

        _queues[u32(queue_type::transfer)].push_back(_device->getQueue(ftransfer, queue_counter[ftransfer]++));
        _command_pools[u32(queue_type::transfer)].push_back(_device->createCommandPool({vk::CommandPoolCreateFlagBits::eTransient, ftransfer}));
        if (enable_present) _queues[u32(queue_type::present)].push_back(_device->getQueue(fpresent, queue_counter[fpresent]++));
        
        VmaAllocatorCreateInfo allocator_create_info{0};
        allocator_create_info.device         = _device.get();
        allocator_create_info.physicalDevice = _gpu;
        allocator_create_info.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
        VmaAllocator alloc;
        vmaCreateAllocator(&allocator_create_info, &alloc);
        _allocator.reset(alloc);
    }

    const queue& graphics_queue(u32 index = 0) const noexcept
    {
        return reinterpret_cast<const queue&>(_queues[u32(queue_type::graphics)][index]);
    }
    const queue& compute_queue(u32 index = 0) const noexcept
    {
        return reinterpret_cast<const queue&>(_queues[u32(queue_type::compute)][index]);
    }
    const queue& transfer_queue() const noexcept { return reinterpret_cast<const queue&>(_queues[u32(queue_type::transfer)][0]); }
    const queue& present_queue() const noexcept { return reinterpret_cast<const queue&>(_queues[u32(queue_type::present)][0]); }

    const u32& graphics_family() const noexcept { return _queue_families[u32(queue_type::graphics)]; }
    const u32& compute_family() const noexcept { return _queue_families[u32(queue_type::compute)]; }
    const u32& transfer_family() const noexcept { return _queue_families[u32(queue_type::transfer)]; }
    const u32& present_family() const noexcept { return _queue_families[u32(queue_type::present)]; }

    vk::Device            dev() const noexcept { return _device.get(); }
    vk::PhysicalDevice    gpu() const noexcept { return _gpu; }
    VmaAllocator          alloc() const noexcept { return _allocator.get(); }
    const ext_dispatcher& dispatcher() const noexcept { return _dispatcher; }
    
    std::vector<commands> allocate_graphics_commands(u32 count, bool primary = true) const noexcept
    {
        vk::CommandBufferAllocateInfo alloc{ _command_pool[u32(queue_type::graphics)].get(), primary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary
            , count};
        auto cmd_bufs = _device->allocateCommandBuffersUnique(alloc);
        return std::move(reinterpret_cast<std::vector<commands>&>(cmd_bufs));
    }
    commands allocate_graphics_command(bool primary = true) const noexcept
    {
        vk::CommandBufferAllocateInfo alloc{ _command_pool[u32(queue_type::graphics)].get(), primary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary
            , count};
        auto cmd_bufs = _device->allocateCommandBuffersUnique(alloc);
        return std::move(reinterpret_cast<commands&>(cmd_bufs[0]));
    }

private:
    u32 presentation_family(instance& i, const surface& s, gsl::span<const vk::QueueFamilyProperties> props) const noexcept
    {
        for (auto fam = 0ull; fam < props.size(); ++fam)
            if (_gpu.getWin32PresentationSupportKHR(fam)) return fam;
        return 0;
    }

    static std::tuple<u32, u32, u32> dedicated_families(gsl::span<const vk::QueueFamilyProperties> props)
    {
        using qp            = vk::QueueFlagBits;
        u32 graphics_family = ~0u;
        u32 compute_family  = ~0u;
        u32 transfer_family = ~0u;
        for (auto i = 0; i < props.size(); ++i)
        {
            if ((props[i].queueFlags & qp::eGraphics) == qp::eGraphics && graphics_family == ~0u) { graphics_family = i; }

            if ((props[i].queueFlags & qp::eCompute) == qp::eCompute)
            {
                if (compute_family == ~0u)
                    compute_family = i;
                else if ((props[i].queueFlags & qp::eGraphics) != qp::eGraphics)
                    compute_family = i;
            }
            if ((props[i].queueFlags & qp::eTransfer) == qp::eTransfer)
            {
                if (transfer_family == ~0u)
                    transfer_family = i;
                else if ((props[i].queueFlags & qp::eGraphics) != qp::eGraphics)
                    transfer_family = i;
            }
        }
        return {graphics_family, compute_family, transfer_family};
    }

    struct vma_alloc_deleter
    {
        void operator()(VmaAllocator alloc) const noexcept { vmaDestroyAllocator(alloc); }
    };

    vk::PhysicalDevice                                 _gpu = nullptr;
    vk::UniqueDevice                                   _device;
    std::unique_ptr<VmaAllocator_T, vma_alloc_deleter> _allocator;
    ext_dispatcher                                     _dispatcher;
    std::array<std::vector<vk::Queue>, 4>              _queues;
    std::array<std::vector<vk::UniqueCommandPool>, 4>  _command_pools;
    std::array<u32, 4>                                 _queue_families{};
};

class swapchain
{
public:
    swapchain(device& d, surface& s, bool general_images = true)
          : _general_images(general_images), _dispatcher(&d.dispatcher()), _device(d.dev()), _gpu(d.gpu()), _surface(s.surf())
    {
        std::unordered_set<u32> families;
        families.emplace(d.present_family());
        if (_general_images)
        {
            families.emplace(d.compute_family());
            families.emplace(d.transfer_family());
            families.emplace(d.graphics_family());
        }
        _condensed_families = std::vector<u32>(families.begin(), families.end());

        recreate(std::nullopt);
    }
    void             recreate() { recreate(*this); }
    vk::SwapchainKHR chain() const noexcept { return _swapchain.get(); }

    u32 count() const noexcept { return _images.size(); }
    
private:
    void recreate(std::optional<std::reference_wrapper<swapchain>> old)
    {
        vk::SwapchainCreateInfoKHR sc;
        sc.clipped        = true;
        sc.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

        const auto scaps = _gpu.getSurfaceCapabilitiesKHR(_surface);
        sc.preTransform  = scaps.currentTransform;
        sc.minImageCount = scaps.minImageCount;
        sc.imageExtent = _extent = scaps.currentExtent;
        sc.imageArrayLayers      = 1;

        using usg     = vk::ImageUsageFlagBits;
        sc.imageUsage = usg::eColorAttachment | usg::eInputAttachment | usg::eTransientAttachment
                        | (_general_images ? usg::eSampled | usg::eStorage | usg::eTransferSrc | usg::eTransferDst : usg{});


        sc.queueFamilyIndexCount = _condensed_families.size();
        sc.pQueueFamilyIndices   = _condensed_families.data();

        _present_mode            = find_present_mode(_gpu.getSurfacePresentModesKHR(_surface));
        const auto [cspace, fmt] = find_image_properties(_gpu.getSurfaceFormatsKHR(_surface));

        sc.presentMode     = _present_mode;
        sc.imageColorSpace = _color_space = cspace;
        sc.imageFormat = _format = fmt;

        sc.imageSharingMode = _condensed_families.size() == 1 ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent;

        if (old) sc.oldSwapchain = (*old).get().chain();
        sc.surface = _surface;

        _swapchain.reset();
        _swapchain = _device.createSwapchainKHRUnique(sc, nullptr, *_dispatcher);

        _images = _device.getSwapchainImagesKHR(_swapchain.get());
    }

    static vk::PresentModeKHR find_present_mode(gsl::span<const vk::PresentModeKHR> modes)
    {
        using pres  = vk::PresentModeKHR;
        pres result = pres::eMailbox;
        for (auto& p : modes)
        {
            if (p == pres::eMailbox)
                return p;
            else if (p == pres::eFifo)
                result = p;
        }
        return result;
    }

    static std::pair<vk::ColorSpaceKHR, vk::Format> find_image_properties(gsl::span<const vk::SurfaceFormatKHR> modes)
    {
        std::pair<vk::ColorSpaceKHR, vk::Format> result{vk::ColorSpaceKHR::eSrgbNonlinear, vk::Format::eB8G8R8A8Unorm};
        for (const auto& sfmt : modes)
        {
            if (sfmt.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear && sfmt.format == vk::Format::eB8G8R8A8Unorm)
                return {vk::ColorSpaceKHR::eSrgbNonlinear, vk::Format::eB8G8R8A8Unorm};
        }
        assert(false);
        return result;
    }

    bool                                               _general_images;
    const ext_dispatcher*                              _dispatcher;
    vk::Device                                         _device;
    vk::PhysicalDevice                                 _gpu;
    vk::SurfaceKHR                                     _surface;
    vk::PresentModeKHR                                 _present_mode;
    vk::ColorSpaceKHR                                  _color_space;
    vk::Format                                         _format;
    vk::UniqueHandle<vk::SwapchainKHR, ext_dispatcher> _swapchain;
    vk::Extent2D                                       _extent;
    std::vector<u32>                                   _condensed_families;
    std::vector<vk::Image>                             _images;
};

}    // namespace v1
}    // namespace gfx

#include <QMainWindow>
#include <QBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QFrame>


void action()
{

}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.resize(1280, 720);
	QMenuBar menu_bar;
	QMenu file_menu;
	QAction* act = file_menu.addAction("Exit");
	window.connect(act, &QAction::triggered, nullptr, []
	{

	});
	menu_bar.addMenu(&file_menu);

	QVBoxLayout mainLayout;
	QWidget central;
	window.setCentralWidget(&central);
	central.setLayout(&mainLayout);
	window.setMenuBar(&menu_bar);
	QWidget surface;
	mainLayout.addWidget(&surface);
	surface.resize(80, 200);

    gfx::instance  my_app("Application", gfx::version_t(1, 0, 0), true, true);
    gfx::surface   surf(my_app, &surface);
    gfx::device    gpu(my_app, gfx::device_target::gpu, {1.f, 0.5f}, 1.f, surf);
    gfx::swapchain chain(gpu, surf);
    
    std::vector<gfx::commands> gpu_cmd = gpu.allocate_graphics_commands(chain.count());
    
    

    

    window.show();

    return app.exec();
}
