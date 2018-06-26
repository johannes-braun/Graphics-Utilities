cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(var_project_name            GFX)
set(var_project_version         0.1.0.0)
set(var_enabled_languages       C CXX)


project(${var_project_name}
    VERSION 
        ${var_project_version}
    LANGUAGES
        ${var_enabled_languages}
)

if(MSVC)
    set(compile_options /MP /openmp /permissive- /Zc:twoPhase- /wd4251)
endif()

set(var_source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(var_include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")

install(DIRECTORY res DESTINATION . 
    FILES_MATCHING
        PATTERN "*"
        PATTERN ".gitignore" EXCLUDE)

add_subdirectory(ext)
add_subdirectory(src)
add_subdirectory(examples)
enable_testing()
add_subdirectory(tests)