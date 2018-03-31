cmake_minimum_required(VERSION 3.9)
project(Graphics)

add_compile_options(/std:c++latest /MP /openmp /permissive- /Zc:twoPhase-)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmt /ignore:4098,4099 /MANIFEST:NO")

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

set(CMAKE_INSTALL_MESSAGE NEVER CACHE STRING "Set the verbosity level for the install messages")
set(EXT_LIB_DEFAULTS -DCMAKE_VERBOSE_MAKEFILE=OFF
    -DCMAKE_INSTALL_MESSAGE=NEVER 
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} 
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>)

include(${PROJECT_SOURCE_DIR}/add_libraries.cmake)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/cached/glshader/include)
list(APPEND LIBRARIES ${EXT_EXTERNAL_LIBRARIES}/cached/glshader/lib/glsp.lib)

list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/cached/glfw3/include)
list(APPEND LIBRARIES ${EXT_EXTERNAL_LIBRARIES}/cached/glfw3/lib/glfw3.lib)

list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/cached/openal-soft/include)
list(APPEND LIBRARIES ${EXT_EXTERNAL_LIBRARIES}/cached/openal-soft/lib/OpenAL32.lib)

list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/cached/assimp/include)
list(APPEND LIBRARIES ${EXT_EXTERNAL_LIBRARIES}/cached/assimp/lib/assimp-vc140-mt.lib)

list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/cached/pugixml/include)
list(APPEND LIBRARIES ${EXT_EXTERNAL_LIBRARIES}/cached/pugixml/lib/pugixml.lib)

list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/vulkan_external/include)
list(APPEND LIBRARIES ${EXT_EXTERNAL_LIBRARIES}/vulkan_external/lib/vulkan-1.lib)
file(COPY ${EXT_EXTERNAL_LIBRARIES}/vulkan_external/bin/glslc.exe DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

list(APPEND LIBRARIES opengl32)  
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/cached/mygl/include)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/cached/glm/include)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/eigen/include)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/asio/include)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/stb/include)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/tinyfd/include)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/nanosvg/include)
add_definitions(-DGLFW_INCLUDE_NONE -DSTBI_MSC_SECURE_CRT -DGLM_FORCE_RADIANS
    -DGLM_ENABLE_EXPERIMENTAL -DIMGUI_INCLUDE_IMGUI_USER_H
    -DASIO_STANDALONE -D_WIN32_WINNT=0x0501 -DNOMINMAX
)

list(APPEND INCLUDES "${EXT_INTERNAL_LIBRARIES}")

file(GLOB children RELATIVE ${EXT_INTERNAL_LIBRARIES} ${EXT_INTERNAL_LIBRARIES}/*)
foreach(subdir ${children})
    if(IS_DIRECTORY ${EXT_INTERNAL_LIBRARIES}/${subdir})
        file(GLOB_RECURSE SRC_SOURCE "${EXT_INTERNAL_LIBRARIES}/${subdir}/*.c" "${EXT_INTERNAL_LIBRARIES}/${subdir}/*.cpp" "${EXT_INTERNAL_LIBRARIES}/${subdir}/*.cc")
        #file(GLOB_RECURSE SRC_HEADERS "${EXT_INTERNAL_LIBRARIES}/${subdir}/*.h" "${EXT_INTERNAL_LIBRARIES}/${subdir}/*.hpp")
        if(NOT SRC_SOURCE)
            # Internal header only library
            set(success 0)
        else()
            add_library(${subdir} ${SRC_SOURCE} ${SRC_HEADERS})
            list(APPEND LIBRARIES ${subdir})
            set_target_properties(${subdir} PROPERTIES LINKER_LANGUAGE CXX)
            target_include_directories(${subdir} PUBLIC ${INCLUDES})
        endif()
    endif()
endforeach()
include_directories(${INCLUDES})

######## Add internal executables
file(GLOB children RELATIVE ${EXT_INTERNAL_EXECUTABLES} ${EXT_INTERNAL_EXECUTABLES}/*)
foreach(subdir ${children})
    if(IS_DIRECTORY ${EXT_INTERNAL_EXECUTABLES}/${subdir})
        file(GLOB_RECURSE SRC_C "${EXT_INTERNAL_EXECUTABLES}/${subdir}/*.c")
        file(GLOB_RECURSE SRC_CPP "${EXT_INTERNAL_EXECUTABLES}/${subdir}/*.cpp")
        #file(GLOB_RECURSE SRC_H "${EXT_INTERNAL_EXECUTABLES}/${subdir}/*.h")
        #file(GLOB_RECURSE SRC_HPP "${EXT_INTERNAL_EXECUTABLES}/${subdir}/*.hpp")
        add_executable(${subdir} ${SRC_C} ${SRC_CPP} ${SRC_H} ${SRC_HPP} "${PROJECT_SOURCE_DIR}/src/res/default_app.rc")
        target_link_libraries(${subdir} ${LIBRARIES})
        set_target_properties(${subdir} PROPERTIES LINKER_LANGUAGE CXX)
        target_include_directories(${subdir} PUBLIC ${INCLUDES})
    endif()
endforeach()