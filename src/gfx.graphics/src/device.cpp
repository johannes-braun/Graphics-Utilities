#if _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "device.hpp"
#include "vulkan/vk_mem_alloc.h"

#include "commands.hpp"
#include "instance.hpp"
#include "sync.hpp"

namespace gfx {
inline namespace v1 {
device::device(instance& i, device_target target, vk::ArrayProxy<const float> graphics_priorities,
               vk::ArrayProxy<const float> compute_priorities, opt_ref<surface> surface,
               vk::ArrayProxy<const char* const> additional_extensions)
      : _instance(&i)
{
    const auto                   gpus        = i.get_instance().enumeratePhysicalDevices();
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

    _enable_present                             = surface && i.is_surface_supported();
    const auto queue_properties                 = _gpu.getQueueFamilyProperties();
    const auto [fgraphics, fcompute, ftransfer] = dedicated_families(queue_properties);
    const auto fpresent                         = _enable_present ? presentation_family(i, *surface, queue_properties) : 0;
    _queue_families[u32(queue_type::graphics)]  = fgraphics;
    _queue_families[u32(queue_type::compute)]   = fcompute;
    _queue_families[u32(queue_type::transfer)]  = ftransfer;
    _queue_families[u32(queue_type::present)]   = fpresent;

    _priorities[fgraphics].insert(_priorities[fgraphics].end(), graphics_priorities.begin(), graphics_priorities.end());
    _priorities[fcompute].insert(_priorities[fcompute].end(), compute_priorities.begin(), compute_priorities.end());
    _priorities[ftransfer].emplace_back(1.f);
    if (_enable_present) _priorities[fpresent].emplace_back(1.f);

    initialize_preset(graphics_priorities.size(), compute_priorities.size(), additional_extensions);
}

device::device(const device& other)
{
    _instance           = other._instance;
    _gpu                = other._gpu;
    _queue_create_infos = other._queue_create_infos;
    _priorities         = other._priorities;
    _queue_families     = other._queue_families;
    _enable_present     = other._enable_present;
    initialize_preset(u32(other._queues[u32(queue_type::graphics)].size()), u32(other._queues[u32(queue_type::compute)].size()), other._extensions);
}

device& device::operator=(const device& other)
{
    _instance           = other._instance;
    _gpu                = other._gpu;
    _queue_create_infos = other._queue_create_infos;
    _priorities         = other._priorities;
    _queue_families     = other._queue_families;
    _enable_present     = other._enable_present;
    initialize_preset(u32(other._queues[u32(queue_type::graphics)].size()), u32(other._queues[u32(queue_type::compute)].size()), other._extensions);
    return *this;
}

const queue& device::graphics_queue(u32 index) const noexcept
{
    return reinterpret_cast<const queue&>(_queues[u32(queue_type::graphics)][index]);
}

const queue& device::compute_queue(u32 index) const noexcept
{
    return reinterpret_cast<const queue&>(_queues[u32(queue_type::compute)][index]);
}

const queue& device::transfer_queue() const noexcept
{
    return reinterpret_cast<const queue&>(_queues[u32(queue_type::transfer)][0]);
}

const queue& device::present_queue() const noexcept
{
    return reinterpret_cast<const queue&>(_queues[u32(queue_type::present)][0]);
}

const u32& device::graphics_family() const noexcept
{
    return _queue_families[u32(queue_type::graphics)];
}

const u32& device::compute_family() const noexcept
{
    return _queue_families[u32(queue_type::compute)];
}

const u32& device::transfer_family() const noexcept
{
    return _queue_families[u32(queue_type::transfer)];
}

const u32& device::present_family() const noexcept
{
    return _queue_families[u32(queue_type::present)];
}

const vk::Device& device::get_device() const noexcept
{
    return _device.get();
}

const vk::PhysicalDevice& device::get_physical_device() const noexcept
{
    return _gpu;
}

allocator device::get_allocator() const noexcept
{
    return _allocator.get();
}

const extension_dispatch& device::get_dispatcher() const noexcept
{
    return _dispatcher;
}

std::vector<commands> device::allocate_graphics_commands(u32 count, bool primary) const noexcept
{
    const vk::CommandBufferAllocateInfo alloc{_command_pools[u32(queue_type::graphics)].get(),
                                              primary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary, count};
    auto                                cmd_bufs = _device->allocateCommandBuffersUnique(alloc);
    return std::move(reinterpret_cast<std::vector<commands>&>(cmd_bufs));
}

commands device::allocate_graphics_command(bool primary) const noexcept
{
    const vk::CommandBufferAllocateInfo alloc{_command_pools[u32(queue_type::graphics)].get(),
                                              primary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary, 1};
    auto                                cmd_bufs = _device->allocateCommandBuffersUnique(alloc);
    return std::move(reinterpret_cast<commands&>(cmd_bufs[0]));
}

std::vector<commands> device::allocate_transfer_commands(u32 count, bool primary) const noexcept
{
    const vk::CommandBufferAllocateInfo alloc{_command_pools[u32(queue_type::transfer)].get(),
                                              primary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary, count};
    auto                                cmd_bufs = _device->allocateCommandBuffersUnique(alloc);
    return std::move(reinterpret_cast<std::vector<commands>&>(cmd_bufs));
}

commands device::allocate_transfer_command(bool primary) const noexcept
{
    const vk::CommandBufferAllocateInfo alloc{_command_pools[u32(queue_type::transfer)].get(),
                                              primary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary, 1};
    auto                                cmd_bufs = _device->allocateCommandBuffersUnique(alloc);
    return std::move(reinterpret_cast<commands&>(cmd_bufs[0]));
}

void device::wait_for(cref_array_view<fence> fences, bool all, std::chrono::nanoseconds timeout)
{
    std::vector<vk::Fence> fn(fences.size());
    for (size_t i = 0; i < fn.size(); ++i) fn[i] = fences.data()[i].get().fen();
    _device->waitForFences(fn, all, timeout.count());
}

void device::reset_fences(cref_array_view<fence> fences)
{
    std::vector<vk::Fence> fn(fences.size());
    for (size_t i = 0; i < fn.size(); ++i) fn[i] = fences.data()[i].get().fen();
    _device->resetFences(fn);
}

u32 device::presentation_family(instance& i, const surface& s, gsl::span<const vk::QueueFamilyProperties> props) const noexcept
{
    for (auto fam = 0ll; fam < props.size(); ++fam)
        if (_gpu.getWin32PresentationSupportKHR(u32(fam))) return u32(fam);
    return 0;
}

void device::initialize_preset(u32 graphics_queue_count, u32 compute_queue_count, vk::ArrayProxy<const char* const> additional_extensions)
{
    const auto fgraphics = _queue_families[u32(queue_type::graphics)];
    const auto fcompute  = _queue_families[u32(queue_type::compute)];
    const auto ftransfer = _queue_families[u32(queue_type::transfer)];
    const auto fpresent  = _queue_families[u32(queue_type::present)];

    auto& graphics_create_info            = _queue_create_infos[fgraphics];
    graphics_create_info.queueFamilyIndex = fgraphics;
    graphics_create_info.queueCount       = u32(_priorities[fgraphics].size());
    graphics_create_info.pQueuePriorities = _priorities[fgraphics].data();
    auto& compute_create_info             = _queue_create_infos[fcompute];
    compute_create_info.queueFamilyIndex  = fcompute;
    compute_create_info.queueCount        = u32(_priorities[fcompute].size());
    compute_create_info.pQueuePriorities  = _priorities[fcompute].data();
    auto& transfer_create_info            = _queue_create_infos[ftransfer];
    transfer_create_info.queueFamilyIndex = ftransfer;
    transfer_create_info.queueCount       = u32(_priorities[ftransfer].size());
    transfer_create_info.pQueuePriorities = _priorities[ftransfer].data();
    if (_enable_present)
    {
        auto& present_create_info            = _queue_create_infos[fpresent];
        present_create_info.queueFamilyIndex = fpresent;
        present_create_info.queueCount       = u32(_priorities[fpresent].size());
        present_create_info.pQueuePriorities = _priorities[fpresent].data();
    }
    std::vector<vk::DeviceQueueCreateInfo> queue_infos(_queue_create_infos.size());
    {
        int index = 0;
        for (auto& p : _queue_create_infos) queue_infos[index++] = p.second;
    }

    vk::PhysicalDeviceFeatures features = _gpu.getFeatures();
    vk::DeviceCreateInfo       device_create_info;
    device_create_info.pEnabledFeatures = &features;

    std::unordered_set<std::string_view> extensions(additional_extensions.begin(), additional_extensions.end());

    extensions.insert({VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME, VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
                       VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME});

    if (_instance->is_surface_supported()) extensions.emplace(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    std::vector<const char*> layers;
    if (_instance->is_debug()) layers.push_back("VK_LAYER_LUNARG_standard_validation");

	_extensions.clear();
	for (const auto& s : extensions)
		_extensions.push_back(s.data());

    device_create_info.enabledExtensionCount   = u32(_extensions.size());
    device_create_info.ppEnabledExtensionNames = _extensions.data();
    device_create_info.enabledLayerCount       = u32(layers.size());
    device_create_info.ppEnabledLayerNames     = layers.data();
    device_create_info.queueCreateInfoCount    = u32(queue_infos.size());
    device_create_info.pQueueCreateInfos       = queue_infos.data();
    _device                                    = _gpu.createDeviceUnique(device_create_info);
    _dispatcher.load(_device.get());
    std::unordered_map<u32, u32> queue_counter;

    for (size_t index = 0; index < graphics_queue_count; ++index)
        _queues[u32(queue_type::graphics)].push_back(_device->getQueue(fgraphics, queue_counter[fgraphics]++));
    _command_pools[u32(queue_type::graphics)] =
        _device->createCommandPoolUnique({vk::CommandPoolCreateFlagBits::eResetCommandBuffer, fgraphics});

    for (size_t index = 0; index < compute_queue_count; ++index)
        _queues[u32(queue_type::compute)].push_back(_device->getQueue(fcompute, queue_counter[fcompute]++));
    _command_pools[u32(queue_type::compute)] =
        _device->createCommandPoolUnique({vk::CommandPoolCreateFlagBits::eResetCommandBuffer, fcompute});

    _queues[u32(queue_type::transfer)].push_back(_device->getQueue(ftransfer, queue_counter[ftransfer]++));
    _command_pools[u32(queue_type::transfer)] = _device->createCommandPoolUnique({vk::CommandPoolCreateFlagBits::eTransient, ftransfer});
    if (_enable_present) _queues[u32(queue_type::present)].push_back(_device->getQueue(fpresent, queue_counter[fpresent]++));

    VmaAllocatorCreateInfo allocator_create_info{0};
    allocator_create_info.device         = _device.get();
    allocator_create_info.physicalDevice = _gpu;
    allocator_create_info.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
    allocator alloc;
    vmaCreateAllocator(&allocator_create_info, &alloc);
    _allocator.reset(alloc);
}

std::tuple<u32, u32, u32> device::dedicated_families(gsl::span<const vk::QueueFamilyProperties> props)
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

void device::vma_alloc_deleter::operator()(allocator alloc) const noexcept
{
    vmaDestroyAllocator(alloc);
}
}    // namespace v1
}    // namespace gfx
