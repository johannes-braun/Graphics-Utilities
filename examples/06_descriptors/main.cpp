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
#include <QLabel>
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
    constexpr version_t(u16 major, u16 minor, u16 patch) : patch(patch), minor(minor), major(major) {}
    constexpr version_t() : version_t(0, 0, 0) {}
    constexpr operator u32() const { return *reinterpret_cast<const u32*>(this); }
    u32       patch : 12, minor : 10, major : 10;
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

    surface(instance& i, QWidget* qt_window) { _surface = i.inst().createWin32SurfaceKHRUnique({{}, nullptr, HWND(qt_window->winId())}); }

    const vk::SurfaceKHR& surf() const noexcept { return _surface.get(); }

private:
    vk::UniqueSurfaceKHR _surface;
};

class commands
{
public:
    const vk::CommandBuffer& cmd() const noexcept { return _buf.get(); }

private:
    vk::UniqueCommandBuffer _buf;
};

class semaphore;
class fence;
class swapchain;

enum class present_error
{
    out_of_date = -1000001004,
    suboptimal  = 1000001003
};
enum class acquire_error
{
    not_ready   = 1,
    timeout     = 2,
    out_of_date = -1000001004,
    suboptimal  = 1000001003
};

template<typename T>
using cref_array_view = vk::ArrayProxy<const std::reference_wrapper<const T>>;
template<typename T>
using opt_ref = std::optional<std::reference_wrapper<T>>;

class queue
{
public:
    queue() = delete;

    void submit(cref_array_view<commands> cmds, cref_array_view<semaphore> wait_for, cref_array_view<semaphore> signal,
                opt_ref<const fence> f = std::nullopt) const;
    [[nodiscard]] std::optional<present_error>
        present(vk::ArrayProxy<const std::pair<u32, const std::reference_wrapper<const swapchain>>> swapchains,
                cref_array_view<semaphore>                                                    wait_for) const;

    const vk::Queue& get() const noexcept { return _queue; }
    void             wait() const noexcept { _queue.waitIdle(); }

private:
    vk::Queue _queue;
};

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
           vk::ArrayProxy<const float> compute_priorities, opt_ref<surface> surface = std::nullopt)
    {
        const auto                   gpus        = i.inst().enumeratePhysicalDevices();
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
            return vdt::eDiscreteGpu;
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
        if (!_gpu)
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
            _queues[u32(queue_type::graphics)].push_back(_device->getQueue(fgraphics, queue_counter[fgraphics]++));
        _command_pools[u32(queue_type::graphics)] =
            _device->createCommandPoolUnique({vk::CommandPoolCreateFlagBits::eResetCommandBuffer, fgraphics});

        for (size_t index = 0; index < compute_priorities.size(); ++index)
            _queues[u32(queue_type::compute)].push_back(_device->getQueue(fcompute, queue_counter[fcompute]++));
        _command_pools[u32(queue_type::compute)] =
            _device->createCommandPoolUnique({vk::CommandPoolCreateFlagBits::eResetCommandBuffer, fcompute});

        _queues[u32(queue_type::transfer)].push_back(_device->getQueue(ftransfer, queue_counter[ftransfer]++));
        _command_pools[u32(queue_type::transfer)] =
            _device->createCommandPoolUnique({vk::CommandPoolCreateFlagBits::eTransient, ftransfer});
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

    const vk::Device&         dev() const noexcept { return _device.get(); }
    const vk::PhysicalDevice& gpu() const noexcept { return _gpu; }
    const VmaAllocator&       alloc() const noexcept { return _allocator.get(); }
    const ext_dispatcher&     dispatcher() const noexcept { return _dispatcher; }

    std::vector<commands> allocate_graphics_commands(u32 count, bool primary = true) const noexcept
    {
        const vk::CommandBufferAllocateInfo alloc{_command_pools[u32(queue_type::graphics)].get(),
                                                  primary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary, count};
        auto                                cmd_bufs = _device->allocateCommandBuffersUnique(alloc);
        return std::move(reinterpret_cast<std::vector<commands>&>(cmd_bufs));
    }
    commands allocate_graphics_command(bool primary = true) const noexcept
    {
        const vk::CommandBufferAllocateInfo alloc{_command_pools[u32(queue_type::graphics)].get(),
                                                  primary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary, 1};
        auto                                cmd_bufs = _device->allocateCommandBuffersUnique(alloc);
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
    std::array<vk::UniqueCommandPool, 4>               _command_pools;
    std::array<u32, 4>                                 _queue_families{};
};

class semaphore
{
public:
    semaphore(device& dev) : _sem(dev.dev().createSemaphoreUnique({})) {}
    const vk::Semaphore& sem() const noexcept { return _sem.get(); }

private:
    vk::UniqueSemaphore _sem;
};

class fence
{
public:
    fence(device& dev, bool signaled = false)
          : _fence(dev.dev().createFenceUnique({signaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags{}}))
    {}
    const vk::Fence& fen() const noexcept { return _fence.get(); }

private:
    vk::UniqueFence _fence;
};

void queue::submit(cref_array_view<commands> cmds, cref_array_view<semaphore> wait_for, cref_array_view<semaphore> signal,
                   opt_ref<const fence> f) const
{
    std::vector<vk::CommandBuffer> submits(cmds.size());
    for (size_t i = 0; i < submits.size(); ++i) submits[i] = cmds.data()[i].get().cmd();
    std::vector<vk::Semaphore> waits(wait_for.size());
    for (size_t i = 0; i < waits.size(); ++i) waits[i] = wait_for.data()[i].get().sem();
    std::vector<vk::Semaphore> sigs(signal.size());
    for (size_t i = 0; i < sigs.size(); ++i) sigs[i] = signal.data()[i].get().sem();

    vk::SubmitInfo submit;
    submit.commandBufferCount   = u32(submits.size());
    submit.pCommandBuffers      = submits.data();
    submit.waitSemaphoreCount   = u32(waits.size());
    submit.pWaitSemaphores      = waits.data();
    vk::PipelineStageFlags flag = vk::PipelineStageFlagBits::eAllCommands;
    submit.pWaitDstStageMask    = &flag;
    submit.signalSemaphoreCount = u32(sigs.size());
    submit.pSignalSemaphores    = sigs.data();
    _queue.submit(submit, f ? f->get().fen() : nullptr);
}

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
    bool                    recreate() { return recreate(*this); }
    const vk::SwapchainKHR& chain() const noexcept { return _swapchain.get(); }

    u32 count() const noexcept { return _images.size(); }

    const std::vector<vk::Image>& imgs() const noexcept { return _images; }

    std::pair<u32, std::optional<acquire_error>> next_image(opt_ref<const semaphore> sem     = std::nullopt,
                                                            opt_ref<const fence>     fen     = std::nullopt,
                                                            std::chrono::nanoseconds timeout = std::chrono::nanoseconds::max())
    {
        auto [result, img] = _device.acquireNextImageKHR(_swapchain.get(), timeout.count(), sem ? sem->get().sem() : nullptr,
                                                         fen ? fen->get().fen() : nullptr);
        return {img, result == vk::Result::eSuccess ? std::nullopt : std::optional<acquire_error>(acquire_error(u32(result)))};
    }

private:
    [[maybe_unused]] bool recreate(std::optional<std::reference_wrapper<swapchain>> old) {
        vk::SwapchainCreateInfoKHR sc;
        sc.clipped        = true;
        sc.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

        vk::SurfaceCapabilitiesKHR scaps{0};
        const auto                 result = _gpu.getSurfaceCapabilitiesKHR(_surface, &scaps);
        if (result == vk::Result::eErrorSurfaceLostKHR) return false;
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
        return true;
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
    const ext_dispatcher*                              _dispatcher = nullptr;
    vk::Device                                         _device;
    vk::PhysicalDevice                                 _gpu;
    vk::SurfaceKHR                                     _surface;
    vk::UniqueHandle<vk::SwapchainKHR, ext_dispatcher> _swapchain;
    std::vector<u32>                                   _condensed_families;
    std::vector<vk::Image>                             _images;
    vk::PresentModeKHR                                 _present_mode = {};
    vk::ColorSpaceKHR                                  _color_space  = {};
    vk::Format                                         _format       = {};
    vk::Extent2D                                       _extent       = {};
};

std::optional<present_error> queue::present(vk::ArrayProxy<const std::pair<u32, const std::reference_wrapper<const swapchain>>> swapchains,
	cref_array_view<semaphore>                                                    wait_for) const
{
	std::vector<u32>              images(swapchains.size());
	std::vector<vk::SwapchainKHR> swaps(swapchains.size());
	for (size_t i = 0; i < swapchains.size(); ++i)
	{
		images[i] = swapchains.data()[i].first;
		swaps[i] = swapchains.data()[i].second.get().chain();
	}
	std::vector<vk::Semaphore> semaphores(wait_for.size());
	for (size_t i = 0; i < swapchains.size(); ++i) semaphores[i] = wait_for.data()[i].get().sem();

	const vk::PresentInfoKHR pr(u32(semaphores.size()), semaphores.data(), u32(swaps.size()), swaps.data(), images.data(), nullptr);
	const vk::Result         r = _queue.presentKHR(&pr);
	return r == vk::Result::eSuccess ? std::nullopt : std::optional<present_error>{ present_error(u32(r)) };
}
}    // namespace v1
}    // namespace gfx

#include <QBoxLayout>
#include <QFrame>
#include <QMainWindow>
#include <QMenuBar>

class worker
{
public:
    template<typename Fun>
    worker(Fun&& fun) : _stopped(false), _worker_thread([&] { fun(*this); })
    {}
    ~worker()
    {
        _stopped = true;
        _worker_thread.join();
    }

    const std::atomic_bool& has_stopped() const noexcept { return _stopped; }
    void                    trigger_stop() noexcept { _stopped = true; }

private:
    std::atomic_bool _stopped;
    std::thread      _worker_thread;
};

#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QSlider>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QMainWindow win;
    win.resize(1280, 720);
    QMenuBar* menuBar  = new QMenuBar();
    QMenu*    fileMenu = new QMenu("File");
    menuBar->addMenu(fileMenu);
    fileMenu->addAction("Save");
    fileMenu->addAction("Exit");

    QHBoxLayout* mainLayout = new QHBoxLayout;
    QWidget*     central    = new QWidget;
    win.setCentralWidget(central);
    central->setLayout(mainLayout);
    mainLayout->setMenuBar(menuBar);
    QFrame* render_frame = new QFrame;
    render_frame->setFrameStyle(QFrame::Shadow_Mask);
    render_frame->setFrameShadow(QFrame::Sunken);
    QVBoxLayout* render_frame_layout = new QVBoxLayout;
    render_frame->setLayout(render_frame_layout);
    QWidget* render_surface = new QWidget;
    render_frame_layout->addWidget(render_surface);
    mainLayout->addWidget(render_frame, 1);

    QVBoxLayout* left_panel_layout = new QVBoxLayout;
    QWidget*     left_panel        = new QWidget;
    mainLayout->addWidget(left_panel);
    left_panel->setLayout(left_panel_layout);
    left_panel_layout->addWidget(new QPushButton("Press me, I'm Qt"));

    QGroupBox*   inputs        = new QGroupBox("Inputs");
    QFormLayout* inputs_layout = new QFormLayout;
    inputs->setLayout(inputs_layout);
    left_panel_layout->addWidget(inputs);
    QSlider* slider_r = new QSlider(Qt::Horizontal);
    QSlider* slider_g = new QSlider(Qt::Horizontal);
    QSlider* slider_b = new QSlider(Qt::Horizontal);
    slider_r->setRange(0, 255);
    slider_g->setRange(0, 255);
    slider_b->setRange(0, 255);
    inputs_layout->addRow(new QLabel("Red"), slider_r);
    inputs_layout->addRow(new QLabel("Green"), slider_g);
    inputs_layout->addRow(new QLabel("Blue"), slider_b);

    std::array<float, 4> clear_color = {0.f, 0.f, 0.f, 1.f};
    win.connect(slider_r, &QSlider::valueChanged, [&](int val) { clear_color[0] = val / 255.f; });
    win.connect(slider_g, &QSlider::valueChanged, [&](int val) { clear_color[1] = val / 255.f; });
    win.connect(slider_b, &QSlider::valueChanged, [&](int val) { clear_color[2] = val / 255.f; });
    win.show();

    gfx::instance              my_app("Application", gfx::version_t(1, 0, 0), true, true);
    gfx::surface               surf1(my_app, render_surface);
    gfx::device                gpu(my_app, gfx::device_target::gpu, {1.f, 0.5f}, 1.f, surf1);
    gfx::swapchain             chain(gpu, surf1);
    std::vector<gfx::commands> gpu_cmd = gpu.allocate_graphics_commands(chain.count());
    gfx::semaphore             acquire_image_signal(gpu);
    gfx::semaphore             render_finish_signal(gpu);
    worker                     render_thread([&](worker& self) {
        while (!self.has_stopped())
        {
            const auto [img, aquire_error] = chain.next_image(acquire_image_signal);
            if (aquire_error && (*aquire_error == gfx::acquire_error::out_of_date || *aquire_error == gfx::acquire_error::suboptimal))
            {
                if (!chain.recreate())
                {
                    gfx::ilog << "Could not recreate swapchain. Exiting.";
                    self.trigger_stop();
                    break;
                }
                continue;
            }

            gpu_cmd[img].cmd().reset({});
            gpu_cmd[img].cmd().begin({vk::CommandBufferUsageFlagBits::eSimultaneousUse});
            gpu_cmd[img].cmd().clearColorImage(chain.imgs()[img], vk::ImageLayout::eUndefined, vk::ClearColorValue{clear_color},
                                               vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
            gpu_cmd[img].cmd().end();

            gpu.graphics_queue().submit({gpu_cmd[img]}, {acquire_image_signal}, {render_finish_signal});
            gpu.graphics_queue().wait();
            const auto present_error = gpu.present_queue().present({{img, chain}}, {render_finish_signal});
            if (present_error)
            {
                if (!chain.recreate())
                {
                    gfx::ilog << "Could not recreate swapchain. Exiting.";
                    self.trigger_stop();
                    break;
                }
                continue;
            }
        }
    });

    return app.exec();
}
