#pragma once

#include "extensions.hpp"
#include "types.hpp"
#include <chrono>
#include <gsl/span>
#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include "allocator.hpp"

namespace gfx {
inline namespace v1 {
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

class instance;
class surface;
class commands;
class queue;
class fence;

class physical_device_handle
{
public:


private:
    vk::PhysicalDevice _gpu;
};

class device
{
public:
    template<typename T>
    friend class mapped;

    explicit device(instance& i, device_target target, vk::ArrayProxy<const float> graphics_priorities,
                    vk::ArrayProxy<const float> compute_priorities, opt_ref<surface> surface = std::nullopt,
                    vk::ArrayProxy<const char* const> additional_extensions = {});

    device( device const& other);
    device(device&& other) noexcept = default;

    device& operator=( device const& other);
    device& operator=(device&& other) noexcept = default;

    ~device() = default;

    [[nodiscard]] auto graphics_queue(u32 index = 0) const noexcept -> queue const&;
    [[nodiscard]] auto compute_queue(u32 index = 0) const noexcept -> queue const&;
    [[nodiscard]] auto transfer_queue() const noexcept -> queue const&;
    [[nodiscard]] auto present_queue() const noexcept -> queue const&;

    [[nodiscard]] auto graphics_family() const noexcept -> u32 const&;
    [[nodiscard]] auto compute_family() const noexcept -> u32 const&;
    [[nodiscard]] auto transfer_family() const noexcept -> u32 const&;
    [[nodiscard]] auto present_family() const noexcept -> u32 const&;

    [[nodiscard]] auto get_device() const noexcept -> vk::Device const&;
    [[nodiscard]] auto get_physical_device() const noexcept -> vk::PhysicalDevice const&;
    [[nodiscard]] auto get_allocator() const noexcept -> allocator;
    [[nodiscard]] auto get_dispatcher() const noexcept -> extension_dispatch const&;

    [[nodiscard]] auto allocate_graphics_commands(u32 count, bool primary = true) const noexcept -> std::vector<commands>;
    [[nodiscard]] auto allocate_graphics_command(bool primary = true) const noexcept -> commands;
    [[nodiscard]] auto allocate_transfer_commands(u32 count, bool primary = true) const noexcept -> std::vector<commands>;
    [[nodiscard]] auto allocate_transfer_command(bool primary = true) const noexcept -> commands;

    void wait_for(cref_array_view<fence> fences, bool all = true, std::chrono::nanoseconds timeout = std::chrono::nanoseconds::max());
    void reset_fences(cref_array_view<fence> fences);

private:
    [[nodiscard]] auto presentation_family(instance& i, surface const& s, gsl::span<vk::QueueFamilyProperties const> props) const noexcept
        -> u32;
    [[nodiscard]] static auto dedicated_families(gsl::span<vk::QueueFamilyProperties const> props) -> std::tuple<u32, u32, u32>;
    void initialize_preset(u32 graphics_queue_count, u32 compute_queue_count, vk::ArrayProxy<char const* const> additional_extensions);

    instance*                                          _instance;
    vk::PhysicalDevice                                 _gpu = nullptr;
    std::unordered_map<u32, vk::DeviceQueueCreateInfo> _queue_create_infos;
    std::unordered_map<u32, std::vector<float>>        _priorities;
    std::array<u32, 4>                                 _queue_families{};
    bool                                               _enable_present = false;
    vk::UniqueDevice                                   _device;
    unique_allocator                                   _allocator;
    extension_dispatch                                 _dispatcher;
    std::array<std::vector<vk::Queue>, 4>              _queues;
    std::array<vk::UniqueCommandPool, 4>               _command_pools;
    std::vector<const char*>                           _extensions;
};
}    // namespace v1
}    // namespace gfx