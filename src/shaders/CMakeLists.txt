macro(compile_spirv var platform)
	set(GLSLC_COMMAND glslc)

	if(IS_ABSOLUTE ${var})
		set(WORK_DIR ${var})
	else()
		get_filename_component(WORK_DIR ./${var} REALPATH)
	endif()
	set(VAR_OUT ${WORK_DIR}.spv-${platform})

	execute_process(COMMAND ${GLSLC_COMMAND} -M "${WORK_DIR}" OUTPUT_VARIABLE FILE_DEPENDENCIES ERROR_VARIABLE FILE_ERROR)
	if(FILE_ERROR)
		message(FATAL_ERROR ${FILE_ERROR})
	endif()
	string(REPLACE " " ";" FILE_DEPENDENCIES_L ${FILE_DEPENDENCIES})
	list(REMOVE_AT FILE_DEPENDENCIES_L 0)
	
	add_custom_command(
		OUTPUT ${VAR_OUT}
		COMMAND ${GLSLC_COMMAND} "${WORK_DIR}" -o "${VAR_OUT}" --target-env=${platform}
		DEPENDS ${FILE_DEPENDENCIES_L})

	list(APPEND SPIRV_${platform} ${VAR_OUT})
endmacro()

macro(compile_spirv_all platform)
	file(GLOB files "*.frag" "*.vert" "*.geom" "*.tese" "*.tesc" "*.comp")
	foreach(file ${files})
		compile_spirv(${file} ${platform})
	endforeach()
endmacro()

macro(create_shaders_target)
	get_filename_component(WORK_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)
	add_custom_target(shaders_${WORK_DIR} DEPENDS ${SPIRV_opengl} ${SPIRV_vulkan})
	unset(SPIRV_opengl)
	unset(SPIRV_vulkan)
endmacro()

compile_spirv(imgui/imgui.frag opengl)
compile_spirv(imgui/imgui.vert opengl)
compile_spirv(imgui/imgui.frag vulkan)
compile_spirv(imgui/imgui.vert vulkan)
compile_spirv(postfx/screen.vert opengl)
compile_spirv(postfx/screen.vert vulkan)
add_custom_target(shaders_gfx DEPENDS ${SPIRV_opengl} ${SPIRV_vulkan})
unset(SPIRV_opengl)
unset(SPIRV_vulkan)