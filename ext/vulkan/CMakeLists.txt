add_library(vulkan STATIC IMPORTED GLOBAL)
set_target_properties(vulkan PROPERTIES 
	IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/vulkan-1.lib
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(vulkan::vulkan ALIAS vulkan)