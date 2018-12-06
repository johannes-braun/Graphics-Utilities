#pragma once

#include "types.hpp"
#include <string>

namespace gfx {
inline namespace v1 {
struct version_t
{
    constexpr static u32 patch_mask = 0x00000FFF;    // Mask back 12 bits
    constexpr static u32 minor_mask = 0x003FF000;    // Mask middle 10 bits
    constexpr static u32 major_mask = 0xFFC00000;    // Mask front 10 bits

    constexpr version_t(u16 major, u16 minor, u16 patch) noexcept : patch(patch), minor(minor), major(major) {}
    constexpr version_t() noexcept : version_t(0, 0, 0) {}

    constexpr u32 packed() const noexcept { return (major << 22) | (minor << 12) | patch; }
    constexpr     operator u32() const noexcept { return (major << 22) | (minor << 12) | patch; }
    u32           patch : 12, minor : 10, major : 10;
};

constexpr version_t to_version(u32 packed) noexcept
{
    return version_t((packed & version_t::major_mask) >> 22, (packed & version_t::minor_mask) >> 12, packed & version_t::patch_mask);
}

inline std::string to_string(const version_t& version)
{
    return std::to_string(version.major) + "." + std::to_string(version.minor) + "." + std::to_string(version.patch);
}

inline std::ostream& operator<<(std::ostream& in, version_t version)
{
    return (in << 'v' << version.major << '.' << version.minor << '.' << version.patch);
}
}    // namespace v1
}    // namespace gfx