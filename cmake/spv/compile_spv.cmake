
function(create_spirv target_name source_dir header_output namespace)
	set(GLOBAL_SPV_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/gfx.shaders/include/)

	unset(CURRENT_DEPENDENCIES)
	file(GLOB_RECURSE gfx_current_vert ${source_dir}/*.vert)
	file(GLOB_RECURSE gfx_current_frag ${source_dir}/*.frag)
	file(GLOB_RECURSE gfx_current_geom ${source_dir}/*.geom)
	file(GLOB_RECURSE gfx_current_tese ${source_dir}/*.tese)
	file(GLOB_RECURSE gfx_current_tesc ${source_dir}/*.tesc)
	file(GLOB_RECURSE gfx_current_comp ${source_dir}/*.comp)

	set(namespace_output "#pragma once\n#include <array>\n#include <cinttypes>\n\nnamespace gfx { inline namespace v1 { namespace spirv {\n")
	set(namespace_output "${namespace_output}    namespace ${namespace} {\n")

	find_program(GLSLC_COMMAND glslc)
	
	foreach(file ${gfx_current_frag} ${gfx_current_vert} ${gfx_current_geom} ${gfx_current_tese} ${gfx_current_tesc} ${gfx_current_comp})
		file(RELATIVE_PATH full_relative_file ${PROJECT_SOURCE_DIR}/ ${file})
		file(RELATIVE_PATH relative_file ${source_dir}/ ${file})

		get_filename_component(current_dir ${relative_file} DIRECTORY)
		get_filename_component(current_filename ${relative_file} NAME)
		string(REPLACE "-" "_" relative_file ${relative_file})
		string(REPLACE " " "_" relative_file ${relative_file})
		string(REPLACE "." "_" relative_file ${relative_file})
		string(REGEX REPLACE "[- .\\/]" "_" full_relative_file ${full_relative_file})
		string(TOUPPER ${full_relative_file} full_relative_file)
		set(full_relative_file GFX_SHADER_SOURCE_${full_relative_file})
		set(${full_relative_file} {0u})
		string(REPLACE "-" "_" current_filename ${current_filename})
		string(REPLACE " " "_" current_filename ${current_filename})
		string(REPLACE "." "_" current_filename ${current_filename})
		if(current_dir)
			string(REPLACE "-" "_" current_dir ${current_dir})
			string(REPLACE " " "_" current_dir ${current_dir})
			string(REPLACE "." "_" current_dir ${current_dir})
			set(iter_list ${current_dir})
			string(REPLACE " " "::" iter_list ${iter_list})
			set(namespace_output "${namespace_output}    namespace ${iter_list} { constexpr uint32_t ${current_filename}[] = @${full_relative_file}@\\\; }\n")
		else()
			set(namespace_output "${namespace_output}    constexpr uint32_t ${current_filename}[] = @${full_relative_file}@\\\;\n")
		endif()

		unset(FILE_DEPENDENCIES)
		execute_process(COMMAND ${GLSLC_COMMAND} -I "${GLOBAL_SPV_INCLUDE_DIR}" -M "${file}" OUTPUT_VARIABLE FILE_DEPENDENCIES ERROR_VARIABLE FILE_ERROR)
		
		if(FILE_ERROR)
			message(FATAL_ERROR ${FILE_ERROR})
		endif()
		string(REPLACE "\n" "" FILE_DEPENDENCIES ${FILE_DEPENDENCIES})
		string(REPLACE " " ";" FILE_DEPENDENCY_LIST ${FILE_DEPENDENCIES})
		list(REMOVE_AT FILE_DEPENDENCY_LIST 0)
		list(APPEND CURRENT_DEPENDENCIES ${FILE_DEPENDENCY_LIST})
	endforeach()

	set(lock_file ${CMAKE_CURRENT_BINARY_DIR}/spv.lock)
	file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/spv.lock)
	add_custom_command(
		OUTPUT ${header_output} ${lock_file}
		COMMAND ${CMAKE_COMMAND} -DGFX_IN=${header_output}.in -DGFX_OUT=${header_output} -DGLOBAL_SPV_INCLUDE_DIR=${GLOBAL_SPV_INCLUDE_DIR} -DGFX_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR} -DGFX_PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR} -DGFX_SORUCE_DIR=${source_dir} -P ${CMAKE_MODULE_PATH}/spv/configure.cmake
		COMMENT "Compiling shaders..."
		DEPENDS ${CURRENT_DEPENDENCIES}
		VERBATIM
	)

	add_custom_target(${target_name}.spirv DEPENDS ${header_output} ${lock_file})

	set(namespace_output ${namespace_output}}}}})
	get_filename_component(out_dir ${header_output} DIRECTORY)
	file(MAKE_DIRECTORY ${out_dir})
	file(WRITE ${header_output}.in ${namespace_output})
	if(NOT EXISTS ${header_output})
		configure_file(${header_output}.in ${header_output} @ONLY)
	endif()

	add_library(${target_name} INTERFACE)
	add_dependencies(${target_name} ${target_name}.spirv)
endfunction()