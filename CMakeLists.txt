cmake_minimum_required(VERSION 3.8)

project(Graphics)

set(AS_EXTENSIONS "${PROJECT_SOURCE_DIR}/cmake")
set(AS_EXTERN_LIBRARIES_PATH "${PROJECT_SOURCE_DIR}/lib")
set(AS_LIBRARIES_PATH "${PROJECT_SOURCE_DIR}/src/lib")
set(AS_EXECUTABLES_PATH "${PROJECT_SOURCE_DIR}/src/exe")

list(APPEND INCLUDES ${AS_EXTERN_LIBRARIES_PATH}/glfw/include)
list(APPEND INCLUDES ${AS_EXTERN_LIBRARIES_PATH}/glad/include)
list(APPEND INCLUDES ${AS_EXTERN_LIBRARIES_PATH}/glm/include)
list(APPEND INCLUDES ${AS_EXTERN_LIBRARIES_PATH}/assimp/include)
list(APPEND INCLUDES ${AS_EXTERN_LIBRARIES_PATH}/al/include)
list(APPEND INCLUDES ${AS_EXTERN_LIBRARIES_PATH}/stb/)
list(APPEND INCLUDES ${AS_EXTERN_LIBRARIES_PATH}/jpu/include)
list(APPEND INCLUDES ${AS_EXTERN_LIBRARIES_PATH}/glpp/include)
list(APPEND INCLUDES ${AS_EXTERN_LIBRARIES_PATH}/imgui/include)
list(APPEND INCLUDES ${AS_EXTERN_LIBRARIES_PATH}/pugixml/include)

add_definitions(
    # GLFW
    -DGLFW_INCLUDE_GLCOREARB 

    # GLM
    -DGLM_FORCE_RADIANS
    -DGLM_FORCE_SWIZZLE
    -DGLM_ENABLE_EXPERIMENTAL

	# ImGui
	-DIMGUI_INCLUDE_IMGUI_USER_H
)

set(BUILD_DIR "${PROJECT_SOURCE_DIR}/build/")
set(AS_BINARY_DIR "${PROJECT_SOURCE_DIR}/build/bin/")
set(AS_LIB_DIR "${PROJECT_SOURCE_DIR}/build/lib/")
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${AS_LIB_DIR}")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${AS_LIB_DIR}")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${AS_BINARY_DIR}")

include("${AS_EXTENSIONS}/subdir.cmake")
include("${AS_EXTENSIONS}/make_lib.cmake")
list(APPEND INCLUDES "${AS_LIBRARIES_PATH}")
collect_subdirectories(PROJECT_LIBRARIES "${AS_LIBRARIES_PATH}")
FOREACH(subdir ${PROJECT_LIBRARIES})
    make_library(${subdir} "${AS_LIBRARIES_PATH}/${subdir}" LIBRARIES)
    set_target_properties(${subdir} PROPERTIES LINKER_LANGUAGE CXX)
    target_include_directories(${subdir} PUBLIC ${INCLUDES})
    file(REMOVE "${BINARY_DIR}/${subdir}.lib")
ENDFOREACH()

file(REMOVE_RECURSE ${AS_BINARY_DIR})
file(MAKE_DIRECTORY ${AS_BINARY_DIR})
file(REMOVE_RECURSE ${AS_LIB_DIR})
file(MAKE_DIRECTORY ${AS_LIB_DIR})
include("${AS_EXTENSIONS}/libraries_all.cmake")

######## Add internal executables
include("${AS_EXTENSIONS}/make_exe.cmake")
include_directories(${AS_LIBRARIES_PATH})
include_directories("${PROJECT_SOURCE_DIR}/src/include")
collect_subdirectories(PROJECT_EXECUTABLES ${AS_EXECUTABLES_PATH})
FOREACH(subdir ${PROJECT_EXECUTABLES})
	make_executable(${subdir} "${AS_EXECUTABLES_PATH}/${subdir}")
	target_link_libraries(${subdir} ${LIBRARIES})
    set_target_properties(${subdir} PROPERTIES LINKER_LANGUAGE CXX)
    target_include_directories(${subdir} PUBLIC ${INCLUDES})
ENDFOREACH()