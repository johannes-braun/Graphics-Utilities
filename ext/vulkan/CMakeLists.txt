add_library(vulkan_internal INTERFACE)
#set(VK_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/alloc
#	${CMAKE_CURRENT_SOURCE_DIR}/headers/include)

#set_target_properties(vulkan PROPERTIES 
#	IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/vulkan-1.lib
#	INTERFACE_INCLUDE_DIRECTORIES "${VK_INCLUDE_DIRS}")

find_package(vulkan-headers REQUIRED)
find_package(vulkan-loader REQUIRED)
find_package(vulkan-validation-layers REQUIRED)

target_link_libraries(vulkan_internal INTERFACE 
	vulkan::headers
	vulkan::loader
	vulkan::validation-layers
	)

add_library(vulkan::vulkan ALIAS vulkan_internal)

get_target_property(_VK_LAYER_PATH VkLayer_core_validation IMPORTED_LOCATION_RELEASE)
get_filename_component(_VK_LAYER_PATH ${_VK_LAYER_PATH} DIRECTORY)
message(${_VK_LAYER_PATH})
target_compile_definitions(vulkan::validation-layers INTERFACE 
	VK_LAYER_PATH="${_VK_LAYER_PATH}")