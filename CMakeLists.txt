cmake_minimum_required(VERSION 3.8)

project(Graphics)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /MP /openmp /EHsc")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmt /ignore:4098,4099 /MANIFEST:NO")

set(AS_EXTENSIONS "${PROJECT_SOURCE_DIR}/cmake")
set(AS_EXTERN_LIBRARIES_PATH "${PROJECT_SOURCE_DIR}/lib")
set(AS_LIBRARIES_PATH "${PROJECT_SOURCE_DIR}/src/lib")
set(AS_EXECUTABLES_PATH "${PROJECT_SOURCE_DIR}/src/exe")

set(EXTERNAL_LIB_DIR "${PROJECT_SOURCE_DIR}/external")

set(BUILD_DIR "${PROJECT_SOURCE_DIR}/build/")
set(AS_BINARY_DIR "${PROJECT_SOURCE_DIR}/build/bin")
set(AS_LIB_DIR "${PROJECT_SOURCE_DIR}/build/lib/")
set(AS_CACHE_DIR "${PROJECT_SOURCE_DIR}/build/cache/")
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${AS_LIB_DIR}")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${AS_LIB_DIR}")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${AS_BINARY_DIR}")

set(EXTERNAL_DEFAULT_EXE_LINKER_FLAGS "/MANIFEST:NO")
set(EXTERNAL_DEFAULT_SHARED_LINKER_FLAGS "/MANIFEST:NO")
set(EXTERNAL_DEFAULT_CMAKE_FLAGS 
    -DCMAKE_VERBOSE_MAKEFILE=ON 
    -DCMAKE_INSTALL_MESSAGE=NEVER 
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} 
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> 
    -DCMAKE_EXE_LINKER_FLAGS=${EXTERNAL_DEFAULT_EXE_LINKER_FLAGS})

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
ExternalProject_Add(glfw3-external
    INSTALL_DIR ${EXTERNAL_LIB_DIR}/cached/glfw3
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/glfw3.lib
    CMAKE_ARGS ${EXTERNAL_DEFAULT_CMAKE_FLAGS} -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_EXAMPLES=OFF)
add_library(libglfw3 STATIC IMPORTED)
add_dependencies(libglfw3 glfw3-external)
set_target_properties(libglfw3 PROPERTIES IMPORTED_LOCATION ${EXTERNAL_LIB_DIR}/cached/glfw3/lib/glfw3.lib)
list(APPEND DEPENDENCIES glfw3-external)
list(APPEND INCLUDES ${EXTERNAL_LIB_DIR}/cached/glfw3/include)
list(APPEND LIBRARIES libglfw3)

ExternalProject_Add(openal-soft-external
    INSTALL_DIR ${EXTERNAL_LIB_DIR}/cached/openal-soft
    GIT_REPOSITORY "https://github.com/kcat/openal-soft.git"
    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/OpenAL32.lib
    CMAKE_ARGS ${EXTERNAL_DEFAULT_CMAKE_FLAGS} -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_RUNTIME_OUTPUT_DIRECTORY} -DALSOFT_UTILS=OFF -DALSOFT_EXAMPLES=OFF -DALSOFT_TESTS=OFF)
add_library(libopenal STATIC IMPORTED)
add_dependencies(libopenal openal-soft-external)
set_target_properties(libopenal PROPERTIES IMPORTED_LOCATION ${EXTERNAL_LIB_DIR}/cached/openal-soft/lib/OpenAL32.lib)
list(APPEND DEPENDENCIES openal-soft-external)
list(APPEND INCLUDES ${EXTERNAL_LIB_DIR}/cached/openal-soft/include)
list(APPEND LIBRARIES libopenal)

ExternalProject_Add(glm-external
    INSTALL_DIR ${EXTERNAL_LIB_DIR}/cached/glm
    GIT_REPOSITORY "https://github.com/g-truc/glm.git"
    CMAKE_ARGS ${EXTERNAL_DEFAULT_CMAKE_FLAGS} -DGLM_TEST_ENABLE_CXX_17=ON)
list(APPEND DEPENDENCIES glm-external)
list(APPEND INCLUDES ${EXTERNAL_LIB_DIR}/cached/glm/include)

string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_uppercase)
if (${CMAKE_BUILD_TYPE_uppercase} MATCHES "DEBUG")
    set(ASSIMP_LIB_NAME "assimp-vc140-mtd")
else()
    set(ASSIMP_LIB_NAME "assimp-vc140-mt")
endif()
ExternalProject_Add(assimp-external
    INSTALL_DIR ${EXTERNAL_LIB_DIR}/cached/assimp
    GIT_REPOSITORY "https://github.com/assimp/assimp.git"
    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/${ASSIMP_LIB_NAME}.lib
    CMAKE_ARGS ${EXTERNAL_DEFAULT_CMAKE_FLAGS}
        -DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_BUILD_TESTS=OFF -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        -DCMAKE_CXX_FLAGS="-DASSIMP_BUILD_NO_EXPORT -DASSIMP_BUILD_NO_AMF_IMPORTER -DASSIMP_BUILD_NO_AC_IMPORTER -DASSIMP_BUILD_NO_ASE_IMPORTER -DASSIMP_BUILD_NO_ASSBIN_IMPORTER -DASSIMP_BUILD_NO_ASSXML_IMPORTER -DASSIMP_BUILD_NO_B3D_IMPORTER -DASSIMP_BUILD_NO_BVH_IMPORTER -DASSIMP_BUILD_NO_DXF_IMPORTER -DASSIMP_BUILD_NO_CSM_IMPORTER -DASSIMP_BUILD_NO_HMP_IMPORTER -DASSIMP_BUILD_NO_IRRMESH_IMPORTER -DASSIMP_BUILD_NO_IRR_IMPORTER -DASSIMP_BUILD_NO_LWO_IMPORTER -DASSIMP_BUILD_NO_LWS_IMPORTER -DASSIMP_BUILD_NO_NFF_IMPORTER -DASSIMP_BUILD_NO_NDO_IMPORTER -DASSIMP_BUILD_NO_OFF_IMPORTER -DASSIMP_BUILD_NO_MS3D_IMPORTER -DASSIMP_BUILD_NO_COB_IMPORTER -DASSIMP_BUILD_NO_IFC_IMPORTER -DASSIMP_BUILD_NO_XGL_IMPORTER -DASSIMP_BUILD_NO_SIB_IMPORTER -DASSIMP_BUILD_NO_3D_IMPORTER -DASSIMP_BUILD_NO_X_IMPORTER -DASSIMP_BUILD_NO_X3D_IMPORTER -DASSIMP_BUILD_NO_GLTF_IMPORTER -DASSIMP_BUILD_NO_3MF_IMPORTER -DASSIMP_BUILD_NO_MMD_IMPORTER")
list(APPEND DEPENDENCIES assimp-external)
add_library(libassimp STATIC IMPORTED)
set_target_properties(libassimp PROPERTIES IMPORTED_LOCATION ${EXTERNAL_LIB_DIR}/cached/assimp/lib/${ASSIMP_LIB_NAME}.lib)
add_dependencies(libassimp assimp-external)
list(APPEND DEPENDENCIES assimp-external)
list(APPEND INCLUDES ${EXTERNAL_LIB_DIR}/cached/assimp/include)
list(APPEND LIBRARIES libassimp)

ExternalProject_Add(snappy-external
    INSTALL_DIR ${EXTERNAL_LIB_DIR}/cached/snappy
    GIT_REPOSITORY "https://github.com/google/snappy.git"
    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/snappy.lib
    CMAKE_ARGS ${EXTERNAL_DEFAULT_CMAKE_FLAGS} -DSNAPPY_BUILD_TESTS=OFF)
add_library(libsnappy STATIC IMPORTED)
add_dependencies(libsnappy snappy-external)
set_target_properties(libsnappy PROPERTIES IMPORTED_LOCATION ${EXTERNAL_LIB_DIR}/cached/snappy/lib/snappy.lib)
list(APPEND DEPENDENCIES snappy-external)
list(APPEND INCLUDES ${EXTERNAL_LIB_DIR}/cached/snappy/include)
list(APPEND LIBRARIES libsnappy)

ExternalProject_Add(pugixml-external
    INSTALL_DIR ${EXTERNAL_LIB_DIR}/cached/pugixml
    GIT_REPOSITORY "https://github.com/zeux/pugixml.git"
    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/pugixml.lib
    CMAKE_ARGS ${EXTERNAL_DEFAULT_CMAKE_FLAGS} -DSNAPPY_BUILD_TESTS=OFF)
add_library(libpugixml STATIC IMPORTED)
add_dependencies(libpugixml pugixml-external)
set_target_properties(libpugixml PROPERTIES IMPORTED_LOCATION ${EXTERNAL_LIB_DIR}/cached/pugixml/lib/pugixml.lib)
list(APPEND DEPENDENCIES pugixml-external)
list(APPEND INCLUDES ${EXTERNAL_LIB_DIR}/cached/pugixml/include)
list(APPEND LIBRARIES libpugixml)

list(APPEND INCLUDES ${EXTERNAL_LIB_DIR}/asio/include)
list(APPEND LIBRARIES ${EXTERNAL_LIB_DIR}/vulkan_external/lib/vulkan-1.lib)
list(APPEND INCLUDES ${EXTERNAL_LIB_DIR}/vulkan_external/include)
file(COPY ${EXTERNAL_LIB_DIR}/vulkan_external/bin/glslc.exe DESTINATION ${AS_BINARY_DIR})
list(APPEND LIBRARIES opengl32)
list(APPEND INCLUDES ${EXTERNAL_LIB_DIR}/glad/include)
list(APPEND INCLUDES ${EXTERNAL_LIB_DIR}/stb/include)


add_definitions(
    -DGLFW_INCLUDE_NONE
    -DSTBI_MSC_SECURE_CRT

    # GLM
    -DGLM_FORCE_RADIANS
    -DGLM_FORCE_SWIZZLE
    -DGLM_ENABLE_EXPERIMENTAL

	# ImGui
	-DIMGUI_INCLUDE_IMGUI_USER_H

    -DASIO_STANDALONE
    -D_WIN32_WINNT=0x0501
    -DNOMINMAX
)

include("${AS_EXTENSIONS}/subdir.cmake")
include("${AS_EXTENSIONS}/make_lib.cmake")
list(APPEND INCLUDES "${AS_LIBRARIES_PATH}")
collect_subdirectories(PROJECT_LIBRARIES "${AS_LIBRARIES_PATH}")
FOREACH(subdir ${PROJECT_LIBRARIES})
    make_library(${subdir} "${AS_LIBRARIES_PATH}/${subdir}" LIBRARIES success)
    if("${success}" STREQUAL "1")
        add_dependencies(${subdir} ${DEPENDENCIES})
        set_target_properties(${subdir} PROPERTIES LINKER_LANGUAGE CXX)
        target_include_directories(${subdir} PUBLIC ${INCLUDES})
    endif()
    file(REMOVE "${BINARY_DIR}/${subdir}.lib")
ENDFOREACH()

file(REMOVE_RECURSE ${PROJECT_SOURCE_DIR}/Debug) # Removes randomly generated Debug folder from MSVC
file(REMOVE_RECURSE ${AS_BINARY_DIR})
file(MAKE_DIRECTORY ${AS_BINARY_DIR})
file(REMOVE_RECURSE ${AS_LIB_DIR})
file(MAKE_DIRECTORY ${AS_LIB_DIR})
file(REMOVE_RECURSE ${AS_CACHE_DIR})
file(MAKE_DIRECTORY ${AS_CACHE_DIR})

######## Add internal executables
include("${AS_EXTENSIONS}/make_exe.cmake")
include_directories(${AS_LIBRARIES_PATH})
include_directories("${PROJECT_SOURCE_DIR}/src/include")
collect_subdirectories(PROJECT_EXECUTABLES ${AS_EXECUTABLES_PATH})
FOREACH(subdir ${PROJECT_EXECUTABLES})
    make_executable(${subdir} "${AS_EXECUTABLES_PATH}/${subdir}")
    add_dependencies(${subdir} ${DEPENDENCIES})
	target_link_libraries(${subdir} ${LIBRARIES})
    set_target_properties(${subdir} PROPERTIES LINKER_LANGUAGE CXX)
    target_include_directories(${subdir} PUBLIC ${INCLUDES})
ENDFOREACH()