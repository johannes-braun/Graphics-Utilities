cmake_minimum_required(VERSION 3.9)
project(GFX)

set(GFX_GLOBAL_FOLDER       ${PROJECT_SOURCE_DIR}/global)
set(GFX_PROJECTS_FOLDER     ${PROJECT_SOURCE_DIR}/projects)
set(GFX_UTILITIES_FOLDER    ${PROJECT_SOURCE_DIR}/utilities)
set(GFX_CMAKE_FOLDER        ${PROJECT_SOURCE_DIR}/cmake)

set(GFX_EXE_PREFIX "exe_")
set(GFX_LIB_PREFIX "lib_")

add_definitions(
    -DGLFW_INCLUDE_NONE -DSTBI_MSC_SECURE_CRT -DGLM_FORCE_RADIANS
    -DGLM_ENABLE_EXPERIMENTAL
    -DASIO_STANDALONE -D_WIN32_WINNT=0x0501 -DNOMINMAX
    -DSOURCE_DIRECTORY="${CMAKE_SOURCE_DIR}/"
)
set_property(GLOBAL PROPERTY CXX_STANDARD 17)

if(MSVC)
    add_compile_options(/MP /openmp /permissive- /Zc:twoPhase-)
    set(GFX_LINKER_FLAGS "/NODEFAULTLIB:libcmt /ignore:4098 /ignore:4099 /MANIFEST:NO")
    list(APPEND GFX_LIBRARIES opengl32)
else()
endif()

set(BUILD_TYPE ${CMAKE_BUILD_TYPE})
if(BUILD_TYPE MATCHES "(R|r?)el")
    set(BUILD_TYPE "Release")
endif()
set_property(GLOBAL PROPERTY LINK_FLAGS ${GFX_LINKER_FLAGS})

file(GLOB INSTALLED_DLLS ${GFX_UTILITIES_FOLDER}/ext/installed/*/${BUILD_TYPE}/bin/*.dll)
file(GLOB INSTALLED_LIBS ${GFX_UTILITIES_FOLDER}/ext/installed/*/${BUILD_TYPE}/lib/*.lib)
file(GLOB INSTALLED_INCLUDES ${GFX_UTILITIES_FOLDER}/ext/installed/*/${BUILD_TYPE}/include)
file(GLOB PREBUILT_DLLS ${GFX_UTILITIES_FOLDER}/ext/prebuilt/*/${BUILD_TYPE}/bin/*.dll)
file(GLOB PREBUILT_LIBS ${GFX_UTILITIES_FOLDER}/ext/prebuilt/*/${BUILD_TYPE}/lib/*.lib)
file(GLOB PREBUILT_INCLUDES ${GFX_UTILITIES_FOLDER}/ext/prebuilt/*/${BUILD_TYPE}/include)
file(GLOB HEADER_ONLY_INCLUDES ${GFX_UTILITIES_FOLDER}/ext/headers/*/include)

list(APPEND GFX_INCLUDE_DIRECTORIES ${PREBUILT_INCLUDES} ${INSTALLED_INCLUDES} ${HEADER_ONLY_INCLUDES})
list(APPEND GFX_DLLS ${PREBUILT_DLLS} ${INSTALLED_DLLS})
list(APPEND GFX_LIBRARIES ${INSTALLED_LIBS} ${PREBUILT_LIBS})

source_group(libraries)

list(APPEND GFX_INCLUDE_DIRECTORIES "${GFX_UTILITIES_FOLDER}/src")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${GFX_UTILITIES_FOLDER}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  "${GFX_UTILITIES_FOLDER}/lib")
file(GLOB children RELATIVE ${GFX_UTILITIES_FOLDER}/src ${GFX_UTILITIES_FOLDER}/src/*)
foreach(subdir ${children})
    if(IS_DIRECTORY ${GFX_UTILITIES_FOLDER}/src/${subdir})
        file(GLOB_RECURSE GFX_SOURCE_FILE "${GFX_UTILITIES_FOLDER}/src/${subdir}/*.c" "${GFX_UTILITIES_FOLDER}/src/${subdir}/*.cpp")
        file(GLOB_RECURSE GFX_HEADER_FILE "${GFX_UTILITIES_FOLDER}/src/${subdir}/*.h" "${GFX_UTILITIES_FOLDER}/src/${subdir}/*.hpp")
        source_group(TREE ${GFX_UTILITIES_FOLDER}/src/ FILES ${GFX_SOURCE_FILE} ${GFX_HEADER_FILE})
        
        if(NOT GFX_SOURCE_FILE)
            # Internal header only library
            set(success 0)
        else()
            add_library(${GFX_LIB_PREFIX}${subdir} ${GFX_SOURCE_FILE} ${GFX_HEADER_FILE})
            list(APPEND GFX_LIBRARIES ${GFX_LIB_PREFIX}${subdir})
            target_include_directories(${GFX_LIB_PREFIX}${subdir} PUBLIC ${GFX_INCLUDE_DIRECTORIES})
            set_target_properties(${GFX_LIB_PREFIX}${subdir} PROPERTIES CXX_STANDARD 17 LINK_FLAGS ${GFX_LINKER_FLAGS})
        endif()
    endif()
endforeach()
include_directories(${GFX_INCLUDE_DIRECTORIES})

######## Add internal executables
file(GLOB children RELATIVE ${GFX_PROJECTS_FOLDER} ${GFX_PROJECTS_FOLDER}/*)
foreach(subdir ${children})
    if(IS_DIRECTORY ${GFX_PROJECTS_FOLDER}/${subdir})
        file(GLOB_RECURSE EXE_SOURCES "${GFX_PROJECTS_FOLDER}/${subdir}/src/*.c" "${GFX_PROJECTS_FOLDER}/${subdir}/src/*.cpp")
        file(GLOB_RECURSE EXE_HEADERS "${GFX_PROJECTS_FOLDER}/${subdir}/src/*.h" "${GFX_PROJECTS_FOLDER}/${subdir}/src/*.hpp")
        if(MSVC)
            file(GLOB WIN_RESOURCES "${GFX_PROJECTS_FOLDER}/${subdir}/src/*.rc")
            if(NOT WIN_RESOURCES)
                set(WIN_RESOURCES "${GFX_GLOBAL_FOLDER}/src/app.rc")
            endif(NOT WIN_RESOURCES)
        endif()

        file(MAKE_DIRECTORY ${GFX_PROJECTS_FOLDER}/${subdir}/src)
        file(MAKE_DIRECTORY ${GFX_PROJECTS_FOLDER}/${subdir}/res)
        file(MAKE_DIRECTORY ${GFX_PROJECTS_FOLDER}/${subdir}/shd)
        file(MAKE_DIRECTORY ${GFX_PROJECTS_FOLDER}/${subdir}/bin)
        
        if(NOT EXISTS ${GFX_PROJECTS_FOLDER}/${subdir}/src/main.cpp)
            message("Creating ${GFX_PROJECTS_FOLDER}/${subdir}/src/main.cpp")
            file(WRITE ${GFX_PROJECTS_FOLDER}/${subdir}/src/main.cpp "int main() {}")
        endif()

        if(NOT EXISTS ${GFX_PROJECTS_FOLDER}/${subdir}/export.xml)
            file(COPY "${GFX_GLOBAL_FOLDER}/src/export_default.xml" DESTINATION ${GFX_PROJECTS_FOLDER}/${subdir})
            file(RENAME "${GFX_PROJECTS_FOLDER}/${subdir}/export_default.xml" ${GFX_PROJECTS_FOLDER}/${subdir}/export.xml)
        endif()

        if(NOT EXISTS ${GFX_PROJECTS_FOLDER}/${subdir}/readme.md)
            file(WRITE ${GFX_PROJECTS_FOLDER}/${subdir}/readme.md "No description available.")
        endif()
        source_group(TREE ${GFX_PROJECTS_FOLDER} FILES ${EXE_HEADERS} ${EXE_SOURCES})

        file(GLOB_RECURSE EXE_SHADERS ${GFX_PROJECTS_FOLDER}/${subdir}/shd/*)
        source_group(TREE ${GFX_PROJECTS_FOLDER} FILES ${EXE_SHADERS})

        add_executable(${GFX_EXE_PREFIX}${subdir} ${EXE_SOURCES} ${EXE_HEADERS} ${WIN_RESOURCES} ${EXE_SHADERS})
        target_link_libraries(${GFX_EXE_PREFIX}${subdir} ${GFX_LIBRARIES})
        file(COPY ${GFX_DLLS} DESTINATION "${GFX_PROJECTS_FOLDER}/${subdir}/bin")
        set_target_properties(${GFX_EXE_PREFIX}${subdir} PROPERTIES LINKER_LANGUAGE CXX RUNTIME_OUTPUT_DIRECTORY "${GFX_PROJECTS_FOLDER}/${subdir}/bin")
        target_include_directories(${GFX_EXE_PREFIX}${subdir} PUBLIC ${GFX_INCLUDE_DIRECTORIES})
        set_target_properties(${GFX_EXE_PREFIX}${subdir} PROPERTIES LINKER_LANGUAGE CXX)
            set_target_properties(${GFX_EXE_PREFIX}${subdir} PROPERTIES CXX_STANDARD 17 LINK_FLAGS ${GFX_LINKER_FLAGS})
    endif()
endforeach()