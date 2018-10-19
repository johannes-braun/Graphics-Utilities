#pragma once

#include <gfx/type.hpp>
#include <string>

namespace gfx
{
    inline namespace v1
    {
		struct version_t
		{
			constexpr version_t(u16 major, u16 minor, u16 patch) : patch(patch), minor(minor), major(major) {}
			constexpr version_t() : version_t(0, 0, 0) {}

			constexpr operator u32() const { return *reinterpret_cast<const u32*>(this); }
			u32       patch : 12, minor : 10, major : 10;
		};

		inline version_t to_version(u32 packed)
		{
			return reinterpret_cast<const version_t&>(packed);
		}

		inline std::string to_string(const version_t& version)
		{
			return std::to_string(version.major) + "." + std::to_string(version.minor) + "." + std::to_string(version.patch);
		}
    }
}