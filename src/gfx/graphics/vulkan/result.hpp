#pragma once

#include <vulkan/vulkan.h>
#include <gfx/log.hpp>

namespace gfx
{
    inline namespace v1
    {
        namespace vulkan
        {
#if defined(NDEBUG)
			constexpr bool do_check_result = false;
#else
			constexpr bool do_check_result = true;
#endif

#define check_result(R) check_result_impl(R, __FILE__, __LINE__, #R)

			inline VkResult check_result_impl(VkResult result, const char* file, int line, const char* fun)
			{
			    if constexpr(do_check_result)
				{
				    if(result != VK_SUCCESS)
				    {
						elog << "Error in " << file << ":" << line << " when calling " << fun;
				    }
					return result;
				}
				else
				{
					return result;
				}
			}
        }
    }
}