set(GLSLC_COMMAND glslc)

set(FLAGS_opengl)
set(FLAGS_vulkan -Dgl_VertexID=gl_VertexIndex)

macro(compile_spirv var platform)
	set(VAR_IN ${var})
	set(VAR_OUT ${var}.spv-${platform})
	get_filename_component(WORK_DIR ${VAR_IN} ABSOLUTE)
	get_filename_component(WORK_DIR ${WORK_DIR} DIRECTORY)
	
	add_custom_command(
		OUTPUT ${VAR_OUT}
		COMMAND ${GLSLC_COMMAND} ${WORK_DIR}/${VAR_IN} -o ${WORK_DIR}/${VAR_OUT} --target-env=${platform} ${FLAGS_${platform}}
		DEPENDS ${VAR_IN})

	list(APPEND SPIRV_${platform} ${VAR_OUT})
endmacro()

macro(create_shaders_target target_name)
	unset(SPIRV_opengl)
	unset(SPIRV_vulkan)
	add_custom_target(${target_name} DEPENDS ${SPIRV_opengl})
endmacro()

add_subdirectory(01_color_normalization)
add_subdirectory(05_implementation_tests)
add_subdirectory(07_meshing)