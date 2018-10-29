include(${CMAKE_ROOT}/Modules/CMakeDetermineCompiler.cmake)

find_program(GLSLC_PATH glslc)
if(NOT GLSLC_PATH)
	find_program(GLSLANGVALIDATOR_PATH glslangvalidator)
	if(NOT GLSLANGVALIDATOR_PATH)
		message( FATAL_ERROR "Could not find glslc nor glslangvalidator.")
	endif()
	set(CMAKE_SPV_COMPILER ${GLSLC_PATH})
else()
	set(CMAKE_SPV_COMPILER ${GLSLC_PATH})
endif()

configure_file(${CMAKE_MODULE_PATH}/CMakeSPVCompiler.cmake.in ${CMAKE_PLATFORM_INFO_DIR}/CMakeSPVCompiler.cmake @ONLY)