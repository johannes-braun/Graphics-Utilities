#if _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "device.hpp"
#include "vulkan/vk_mem_alloc.h"

#include "instance.hpp"
#include "commands.hpp"
#include "sync.hpp"
#include <unordered_map>

namespace gfx
{
	inline namespace v1
	{

	device::device(instance&                   i, device_target                     target, vk::ArrayProxy<const float> graphics_priorities,
	               vk::ArrayProxy<const float> compute_priorities, opt_ref<surface> surface) : _instance(&i)
	{
	    const auto                   gpus        = i.inst().enumeratePhysicalDevices();
	    const vk::PhysicalDeviceType target_type = [target]
	    {
	        using dt = device_target;
	        using vdt = vk::PhysicalDeviceType;
	        switch (target) {
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
	    for (const auto&   gpu : gpus) {
	        if (gpu.getProperties().deviceType == target_type) {
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
	    if (!_gpu) {
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
	    const auto fpresent                         = enable_present
	                                                                                              ? presentation_family(
	                                                                                                  i, *surface, queue_properties)
	                                                                                              : 0;
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
	    if (enable_present) {
	        auto& present_create_info            = queue_create_infos[fpresent];
	        present_create_info.queueFamilyIndex = fpresent;
	        present_create_info.queueCount       = priorities[fpresent].size();
	        present_create_info.pQueuePriorities = priorities[fpresent].data();
	    }
	    std::vector<vk::DeviceQueueCreateInfo> queue_infos(queue_create_infos.size());
	    {
	        int        index                                        = 0;
	        for (auto& p : queue_create_infos) queue_infos[index++] = p.second;
	    }

	    vk::PhysicalDeviceFeatures features = _gpu.getFeatures();
	    vk::DeviceCreateInfo       device_create_info;
	    device_create_info.pEnabledFeatures = &features;

	    std::vector<const char*> extensions{
	        VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME, VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
	        VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME
	    };
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
		allocator alloc;
	    vmaCreateAllocator(&allocator_create_info, &alloc);
	    _allocator.reset(alloc);
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

	const vk::Device& device::dev() const noexcept
	{
	    return _device.get();
	}

	const vk::PhysicalDevice& device::gpu() const noexcept
	{
	    return _gpu;
	}

	allocator device::alloc() const noexcept
	{
	    return _allocator.get();
	}

	const extension_dispatch& device::dispatcher() const noexcept
	{
	    return _dispatcher;
	}

	std::vector<commands> device::allocate_graphics_commands(u32 count, bool primary) const noexcept
	{
	    const vk::CommandBufferAllocateInfo alloc{
	        _command_pools[u32(queue_type::graphics)].get(),
	        primary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary, count
	    };
	    auto cmd_bufs = _device->allocateCommandBuffersUnique(alloc);
	    return std::move(reinterpret_cast<std::vector<commands>&>(cmd_bufs));
	}

	commands device::allocate_graphics_command(bool primary) const noexcept
	{
	    const vk::CommandBufferAllocateInfo alloc{
	        _command_pools[u32(queue_type::graphics)].get(),
	        primary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary, 1
	    };
	    auto cmd_bufs = _device->allocateCommandBuffersUnique(alloc);
	    return std::move(reinterpret_cast<commands&>(cmd_bufs[0]));
	}

	void device::wait_for(cref_array_view<fence> fences, bool all, std::chrono::nanoseconds timeout)
	{
		std::vector<vk::Fence> fn(fences.size());
		for (size_t i = 0; i < fn.size(); ++i)
			fn[i] = fences.data()[i].get().fen();
		_device->waitForFences(fn, all, timeout.count());
	}

	void device::reset_fences(cref_array_view<fence> fences)
	{
		std::vector<vk::Fence> fn(fences.size());
		for (size_t i = 0; i < fn.size(); ++i)
			fn[i] = fences.data()[i].get().fen();
		_device->resetFences(fn);
	}

	u32 device::presentation_family(instance& i, const surface& s, gsl::span<const vk::QueueFamilyProperties> props) const noexcept
	{
	    for (auto fam = 0ull; fam < props.size(); ++fam)
	        if (_gpu.getWin32PresentationSupportKHR(fam)) return fam;
	    return 0;
	}

	std::tuple<u32, u32, u32> device::dedicated_families(gsl::span<const vk::QueueFamilyProperties> props)
	{
	    using qp = vk::QueueFlagBits;
	    u32       graphics_family = ~0u;
	    u32       compute_family  = ~0u;
	    u32       transfer_family = ~0u;
	    for (auto i               = 0; i < props.size();
	         ++i) {
	        if ((props[i].queueFlags & qp::eGraphics) == qp::eGraphics && graphics_family == ~0u) { graphics_family = i; }

	        if ((props[i].queueFlags & qp::eCompute) == qp::eCompute) {
	            if (compute_family == ~0u)
	                compute_family = i;
	            else if ((props[i].queueFlags & qp::eGraphics) != qp::eGraphics)
	                compute_family = i;
	        }
	        if ((props[i].queueFlags & qp::eTransfer) == qp::eTransfer) {
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
	}
}
