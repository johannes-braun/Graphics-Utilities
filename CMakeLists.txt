cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(var_project_name            GFX)
set(var_project_version         0.1.0.0)
set(var_enabled_languages       C CXX)

option(GFX_USE_INSTALL "Enable install targets" OFF)
option(GFX_USE_TESTS   "Enable test targets"	ON)

macro(gfx_msg)
	message("GFX -- " ${ARGV})
endmacro()

project(${var_project_name}
    VERSION 
        ${var_project_version}
    LANGUAGES
        ${var_enabled_languages}
)

if(MSVC)
    set(compile_options -MP -openmp -permissive- -Zc:twoPhase-)
else()
    set(compile_options -Wnon-template-friend -Wclass-memaccess -Wtautological-compare -Wconversion-null -std=c++1z)
endif()

set(var_source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(var_include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(GFX_USE_INSTALL)
	install(DIRECTORY res DESTINATION . 
		FILES_MATCHING
			PATTERN "*"
			PATTERN ".gitignore" EXCLUDE)
endif()

add_subdirectory(ext)
add_subdirectory(src)
add_subdirectory(examples)
if(GFX_USE_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()