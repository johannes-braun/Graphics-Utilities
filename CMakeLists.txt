cmake_minimum_required(VERSION 3.8)
project(Graphics)

set(MSVC_COMPILE_FLAGS /std:c++latest /MP /openmp)
set(MSVC_LINKER_FLAGS /NODEFAULTLIB:libcmt /ignore:4098,4099 /MANIFEST:NO)

set(EXT_CMAKE_MODULE_PATH           "${PROJECT_SOURCE_DIR}/cmake")
set(EXT_EXTERNAL_LIBRARIES          "${PROJECT_SOURCE_DIR}/external")
set(EXT_INTERNAL_LIBRARIES          "${PROJECT_SOURCE_DIR}/src/lib")
set(EXT_INTERNAL_EXECUTABLES        "${PROJECT_SOURCE_DIR}/src/exe")
set(EXT_BUILD_ROOT                  "${PROJECT_SOURCE_DIR}/build")
set(EXT_CACHE_PATH                  "${PROJECT_SOURCE_DIR}/build/cache")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${EXT_BUILD_ROOT}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  "${EXT_BUILD_ROOT}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${EXT_BUILD_ROOT}/bin")

file(REMOVE_RECURSE ${PROJECT_SOURCE_DIR}/Debug) # Removes randomly generated Debug folder from MSVC
file(REMOVE_RECURSE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ${EXT_CACHE_PATH})
file(MAKE_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ${EXT_CACHE_PATH})

add_compile_options(${MSVC_COMPILE_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MSVC_LINKER_FLAGS}")

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
macro(load_git_library lib_name lib_repo lib_file cmake_args)
    ExternalProject_Add(${lib_name}-external
        GIT_REPOSITORY ${lib_repo}
        INSTALL_DIR ${EXT_EXTERNAL_LIBRARIES}/cached/${lib_name}
        BUILD_BYPRODUCTS <INSTALL_DIR>/lib/${lib_file}
        CMAKE_ARGS -DCMAKE_VERBOSE_MAKEFILE=ON 
            -DCMAKE_INSTALL_MESSAGE=NEVER 
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} 
            -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> ${cmake_args})
    add_library(lib${lib_name} STATIC IMPORTED)
    add_dependencies(lib${lib_name} ${lib_name}-external)
    set_target_properties(lib${lib_name} PROPERTIES IMPORTED_LOCATION ${EXT_EXTERNAL_LIBRARIES}/cached/${lib_name}/lib/${lib_file})

    list(APPEND DEPENDENCIES ${lib_name}-external)
    list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/cached/${lib_name}/include)
    list(APPEND LIBRARIES lib${lib_name})
endmacro()


load_git_library(glfw3 "https://github.com/glfw/glfw.git" glfw3.lib
    -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_EXAMPLES=OFF)

load_git_library(openal-soft "https://github.com/kcat/openal-soft.git" OpenAL32.lib
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_RUNTIME_OUTPUT_DIRECTORY} 
    -DALSOFT_UTILS=OFF -DALSOFT_EXAMPLES=OFF -DALSOFT_TESTS=OFF)

load_git_library(assimp "https://github.com/assimp/assimp.git" assimp-vc140-mt.lib
    -DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_BUILD_TESTS=OFF -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

load_git_library(snappy "https://github.com/google/snappy.git" snappy.lib -DSNAPPY_BUILD_TESTS=OFF)
load_git_library(pugixml "https://github.com/zeux/pugixml.git" pugixml.lib "")

ExternalProject_Add(glm-external INSTALL_DIR ${EXT_EXTERNAL_LIBRARIES}/cached/glm GIT_REPOSITORY "https://github.com/g-truc/glm.git" CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXT_EXTERNAL_LIBRARIES}/cached/glm)
list(APPEND DEPENDENCIES glm-external)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/cached/glm/include)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/asio/include)
list(APPEND LIBRARIES ${EXT_EXTERNAL_LIBRARIES}/vulkan_external/lib/vulkan-1.lib)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/vulkan_external/include)
file(COPY ${EXT_EXTERNAL_LIBRARIES}/vulkan_external/bin/glslc.exe DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
list(APPEND LIBRARIES opengl32) 
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/glad/include)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/stb/include)
add_definitions(-DGLFW_INCLUDE_NONE -DSTBI_MSC_SECURE_CRT -DGLM_FORCE_RADIANS
    -DGLM_FORCE_SWIZZLE -DGLM_ENABLE_EXPERIMENTAL -DIMGUI_INCLUDE_IMGUI_USER_H
    -DASIO_STANDALONE -D_WIN32_WINNT=0x0501 -DNOMINMAX
)

list(APPEND INCLUDES "${EXT_INTERNAL_LIBRARIES}")
include_directories(${INCLUDES})

file(GLOB children RELATIVE ${EXT_INTERNAL_LIBRARIES} ${EXT_INTERNAL_LIBRARIES}/*)
foreach(subdir ${children})
    if(IS_DIRECTORY ${EXT_INTERNAL_LIBRARIES}/${subdir})
        file(GLOB_RECURSE SRC_SOURCE "${EXT_INTERNAL_LIBRARIES}/${subdir}/*.c" "${EXT_INTERNAL_LIBRARIES}/${subdir}/*.cpp" "${EXT_INTERNAL_LIBRARIES}/${subdir}/*.cc")
        file(GLOB_RECURSE SRC_HEADERS "${EXT_INTERNAL_LIBRARIES}/${subdir}/*.h" "${EXT_INTERNAL_LIBRARIES}/${subdir}/*.hpp")
        if(NOT SRC_SOURCE)
            # Internal header only library
            set(success 0)
        else()
            add_library(${subdir} ${SRC_SOURCE} ${SRC_HEADERS})
            list(APPEND LIBRARIES ${subdir})
            add_dependencies(${subdir} ${DEPENDENCIES})
            set_target_properties(${subdir} PROPERTIES LINKER_LANGUAGE CXX)
            target_include_directories(${subdir} PUBLIC ${INCLUDES})
        endif()
    endif()
endforeach()

######## Add internal executables
file(GLOB children RELATIVE ${EXT_INTERNAL_EXECUTABLES} ${EXT_INTERNAL_EXECUTABLES}/*)
foreach(subdir ${children})
    if(IS_DIRECTORY ${EXT_INTERNAL_EXECUTABLES}/${subdir})
        file(GLOB_RECURSE SRC_C "${EXT_INTERNAL_EXECUTABLES}/${subdir}/*.c")
        file(GLOB_RECURSE SRC_CPP "${EXT_INTERNAL_EXECUTABLES}/${subdir}/*.cpp")
        file(GLOB_RECURSE SRC_H "${EXT_INTERNAL_EXECUTABLES}/${subdir}/*.h")
        file(GLOB_RECURSE SRC_HPP "${EXT_INTERNAL_EXECUTABLES}/${subdir}/*.hpp")
        add_executable(${subdir} ${SRC_C} ${SRC_CPP} ${SRC_H} ${SRC_HPP} "${PROJECT_SOURCE_DIR}/src/res/default_app.rc")
        add_dependencies(${subdir} ${DEPENDENCIES})
        target_link_libraries(${subdir} ${LIBRARIES})
        set_target_properties(${subdir} PROPERTIES LINKER_LANGUAGE CXX)
        target_include_directories(${subdir} PUBLIC ${INCLUDES})
    endif()
endforeach()