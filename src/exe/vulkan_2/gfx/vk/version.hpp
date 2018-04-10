#pragma once

#include <vulkan/vulkan.h>

namespace gfx::vk
{
    struct version {
        constexpr version(uint32_t major, uint32_t minor, uint32_t patch = 0) noexcept : major(major), minor(minor), patch(patch) {}
        constexpr version(uint32_t packed) noexcept : major(packed>>22), minor((packed>>12) & 0x3ff), patch(packed & 0xfff) {}

        uint32_t patch : 12;
        uint32_t minor : 10;
        uint32_t major : 10;
    };
    static_assert(sizeof(version) == sizeof(uint32_t), "Version type size mismatch!");
}