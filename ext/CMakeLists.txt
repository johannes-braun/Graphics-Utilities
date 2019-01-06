function(log_external target)
	if(NOT TARGET ${target})
		message(WARNING "[WARNING] Could not find: ${target}")
	endif()
endfunction()

macro(make_global target name)
	log_external(${target})
	if(TARGET ${target})
		message("[TARGET] Creating alias gfx_ext::${name} -> ${target}")
		add_library(ext_${name} INTERFACE IMPORTED GLOBAL)
		target_link_libraries(ext_${name} INTERFACE ${target})
		add_library(gfx_ext::${name} ALIAS ext_${name})
	endif()
endmacro()


find_package(gslib QUIET)						# Defines:  GSL
find_package(vulkan-headers QUIET)				# Defines:	vulkan::headers
find_package(vulkan-loader QUIET)				# Defines:	vulkan::loader
find_package(vulkan-validation-layers QUIET)	# Defines:	vulkan::validation-layers
find_package(vulkan-memory-allocator QUIET)		# Defines:	vulkan::memory-allocator
find_package(range-v3 QUIET)					# Defines:	range-v3
find_package(glfw3 QUIET)						# Defines:	glfw
find_package(glm QUIET)							# Defines:  glm
find_package(Eigen3 QUIET)						# Defines:  Eigen3::Eigen
find_package(Qt5Core QUIET)						# Defines:	Qt5::Core
find_package(Qt5Widgets QUIET)					# Defines:	Qt5::Widgets
find_package(Qt5Gui QUIET)						# Defines:	Qt5::Gui
find_package(Qt5Concurrent QUIET)				# Defines:	Qt5::Concurrent
find_package(Qt5Charts QUIET)				# Defines:	Qt5::Concurrent
find_package(glsp QUIET)						# Defines:	jbraun::glsp
find_package(assimp QUIET)
add_subdirectory(mygl)							# Defines:	jbraun::mygl

make_global(GSL							gsl)
make_global(vulkan::headers				vk_headers)
make_global(vulkan::loader				vk_loader)
make_global(vulkan::validation-layers	vk_validation)
make_global(vulkan::memory-allocator	vk_mem_alloc)
make_global(range-v3					range-v3)
make_global(glfw						glfw)
make_global(glm							glm)
make_global(Eigen3::Eigen				eigen3)
make_global(Qt5::Core					qt5_core)
make_global(Qt5::Widgets				qt5_widgets)
make_global(Qt5::Gui					qt5_gui)
make_global(Qt5::Concurrent				qt5_concurrent)
make_global(Qt5::Charts					qt5_charts)
make_global(jbraun::glsp				glsp)
make_global(jbraun::mygl				mygl)

if(${assimp_FOUND})
	message("[TARGET] Creating alias gfx_ext::assimp -> ASSIMP_LIBRARIES")
	add_library(ext_assimp INTERFACE IMPORTED GLOBAL)
	target_link_libraries(ext_assimp INTERFACE ${ASSIMP_LIBRARIES})
	add_library(gfx_ext::assimp ALIAS ext_assimp)
else()
	message(WARNING "[WARNING] Could not find: assimp")
endif()

# Set VK_LAYER_PATH
get_target_property(_VK_LAYER_PATH VkLayer_core_validation IMPORTED_LOCATION_RELEASE)
get_filename_component(_VK_LAYER_PATH ${_VK_LAYER_PATH} DIRECTORY)
target_compile_definitions(vulkan::validation-layers INTERFACE VK_LAYER_PATH="${_VK_LAYER_PATH}")