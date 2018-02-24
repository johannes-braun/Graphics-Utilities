cmake_minimum_required(VERSION 3.8)
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
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
set(EXT_LIB_DEFAULTS -DCMAKE_VERBOSE_MAKEFILE=OFF
    -DCMAKE_INSTALL_MESSAGE=NEVER 
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} 
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>)
macro(load_git_library lib_name lib_repo lib_file git_tag)
    file(REMOVE_RECURSE ${EXT_EXTERNAL_LIBRARIES}/cached/${lib_name})
    ExternalProject_Add(${lib_name}-external
        GIT_REPOSITORY ${lib_repo}
        GIT_TAG ${git_tag}
        UPDATE_COMMAND "" 
        INSTALL_DIR ${EXT_EXTERNAL_LIBRARIES}/cached/${lib_name}
        BUILD_BYPRODUCTS <INSTALL_DIR>/lib/${lib_file}
        BUILD_ALWAYS 0
        LOG_DOWNLOAD 0
        LOG_UPDATE 0
        LOG_DOWNLOAD LOG_DOWNLOAD  0
        LOG_BUILD 0
        LOG_TEST 0
        LOG_INSTALL 0
        CMAKE_ARGS ${EXT_LIB_DEFAULTS} ${EXT_LIB_ARGS})
    add_library(lib${lib_name} STATIC IMPORTED)
    add_dependencies(lib${lib_name} ${lib_name}-external)
    set_target_properties(lib${lib_name} PROPERTIES IMPORTED_LOCATION ${EXT_EXTERNAL_LIBRARIES}/cached/${lib_name}/lib/${lib_file})

    set(EXT_LIB_ARGS "")
    list(APPEND DEPENDENCIES ${lib_name}-external)
    list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/cached/${lib_name}/include)
    list(APPEND LIBRARIES lib${lib_name})
endmacro()

set(EXT_LIB_ARGS 
    -DGLFW_BUILD_TESTS=OFF 
    -DGLFW_BUILD_DOCS=OFF 
    -DGLFW_BUILD_EXAMPLES=OFF
)
load_git_library(glfw3 "https://github.com/glfw/glfw.git" glfw3.lib master)

set(EXT_LIB_ARGS 
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_RUNTIME_OUTPUT_DIRECTORY} 
    -DALSOFT_UTILS=OFF 
    -DALSOFT_EXAMPLES=OFF 
    -DALSOFT_TESTS=OFF
)
load_git_library(openal-soft "https://github.com/kcat/openal-soft.git" OpenAL32.lib master)

string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_uppercase)
IF(${CMAKE_BUILD_TYPE_uppercase} MATCHES "DEBUG")
    set(ASSIMP_LIB_FILE "assimp-vc140-mt.lib")
else()
    set(ASSIMP_LIB_FILE "assimp-vc140-mt.lib")
ENDIF()

set(ASSIMP_CXX_FLAGS 
    /bigobj 
    /DASSIMP_BUILD_NO_EXPORT 
    /DASSIMP_BUILD_NO_AMF_IMPORTER 
    /DASSIMP_BUILD_NO_AC_IMPORTER 
    /DASSIMP_BUILD_NO_ASE_IMPORTER 
    /DASSIMP_BUILD_NO_ASSBIN_IMPORTER 
    /DASSIMP_BUILD_NO_ASSXML_IMPORTER 
    /DASSIMP_BUILD_NO_B3D_IMPORTER 
    /DASSIMP_BUILD_NO_BVH_IMPORTER 
    /DASSIMP_BUILD_NO_DXF_IMPORTER 
    /DASSIMP_BUILD_NO_CSM_IMPORTER 
    /DASSIMP_BUILD_NO_HMP_IMPORTER 
    /DASSIMP_BUILD_NO_IRRMESH_IMPORTER 
    /DASSIMP_BUILD_NO_IRR_IMPORTER 
    /DASSIMP_BUILD_NO_LWO_IMPORTER 
    /DASSIMP_BUILD_NO_LWS_IMPORTER 
    /DASSIMP_BUILD_NO_NFF_IMPORTER 
    /DASSIMP_BUILD_NO_NDO_IMPORTER 
    /DASSIMP_BUILD_NO_OFF_IMPORTER 
    /DASSIMP_BUILD_NO_MS3D_IMPORTER 
    /DASSIMP_BUILD_NO_COB_IMPORTER 
    /DASSIMP_BUILD_NO_IFC_IMPORTER 
    /DASSIMP_BUILD_NO_XGL_IMPORTER 
    /DASSIMP_BUILD_NO_SIB_IMPORTER 
    /DASSIMP_BUILD_NO_3D_IMPORTER 
    /DASSIMP_BUILD_NO_X_IMPORTER 
    /DASSIMP_BUILD_NO_X3D_IMPORTER 
    /DASSIMP_BUILD_NO_GLTF_IMPORTER 
    /DASSIMP_BUILD_NO_3MF_IMPORTER 
    /DASSIMP_BUILD_NO_MMD_IMPORTER)
set(EXT_LIB_ARGS 
    -DCMAKE_CXX_FLAGS=${ASSIMP_CXX_FLAGS}
    -DASSIMP_BUILD_ASSIMP_TOOLS=OFF 
    -DASSIMP_BUILD_TESTS=OFF
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
load_git_library(assimp "https://github.com/assimp/assimp.git" ${ASSIMP_LIB_FILE} master)

set(EXT_LIB_ARGS -DSNAPPY_BUILD_TESTS=OFF)
load_git_library(snappy "https://github.com/google/snappy.git" snappy.lib master)
load_git_library(pugixml "https://github.com/zeux/pugixml.git" pugixml.lib master)


ExternalProject_Add(eigen-external INSTALL_DIR ${EXT_EXTERNAL_LIBRARIES}/cached/eigen GIT_REPOSITORY "https://github.com/eigenteam/eigen-git-mirror.git" GIT_TAG master UPDATE_COMMAND "" 
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXT_EXTERNAL_LIBRARIES}/cached/eigen -DCMAKE_INSTALL_MESSAGE=NEVER -DBUILD_TESTING=OFF)
list(APPEND DEPENDENCIES eigen-external)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/cached/eigen/include)

ExternalProject_Add(glm-external INSTALL_DIR ${EXT_EXTERNAL_LIBRARIES}/cached/glm GIT_REPOSITORY "https://github.com/g-truc/glm.git" GIT_TAG master UPDATE_COMMAND "" 
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXT_EXTERNAL_LIBRARIES}/cached/glm -DCMAKE_INSTALL_MESSAGE=NEVER)
list(APPEND DEPENDENCIES glm-external)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/cached/glm/include)

list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/asio/include)
list(APPEND LIBRARIES ${EXT_EXTERNAL_LIBRARIES}/vulkan_external/lib/vulkan-1.lib)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/vulkan_external/include)
file(COPY ${EXT_EXTERNAL_LIBRARIES}/vulkan_external/bin/glslc.exe DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
list(APPEND LIBRARIES opengl32)  
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/glad/include)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/stb/include)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/tinyfd/include)
list(APPEND INCLUDES ${EXT_EXTERNAL_LIBRARIES}/nanosvg/include)
add_definitions(-DGLFW_INCLUDE_NONE -DSTBI_MSC_SECURE_CRT -DGLM_FORCE_RADIANS
    -DGLM_ENABLE_EXPERIMENTAL -DIMGUI_INCLUDE_IMGUI_USER_H
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