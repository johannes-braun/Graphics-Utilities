add_library(vulkan STATIC IMPORTED GLOBAL)
set(VK_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/alloc
	${CMAKE_CURRENT_SOURCE_DIR}/headers/include)

set_target_properties(vulkan PROPERTIES 
	IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/vulkan-1.lib
	INTERFACE_INCLUDE_DIRECTORIES "${VK_INCLUDE_DIRS}")
add_library(vulkan::vulkan ALIAS vulkan)