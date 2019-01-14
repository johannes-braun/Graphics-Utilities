
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_BINARY_DIR}>)

set(GFX_APP_RESOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/res/)
if(WIN32)
    set(GFX_APP_RESOURCES ${GFX_APP_RESOURCE_PATH}/app.rc)
endif()

function(mark_as_executable exe_name)
    file(GLOB_RECURSE CPPS *.cpp *.natvis)
	add_executable(${exe_name} ${CPPS})
	set_target_properties(${exe_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/${exe_name})
    if(WIN32)
        target_sources(${exe_name} PUBLIC ${GFX_APP_RESOURCE_PATH}/app.rc)
    endif()
    target_compile_features(${exe_name} PRIVATE cxx_std_17)
	if(WIN32)
		target_link_libraries(${exe_name} PRIVATE comctl32)
	endif()
    target_compile_options(${exe_name} PRIVATE ${compile_options})
   # set_target_properties(${exe_name} PROPERTIES LINK_FLAGS_RELEASE "-subsystem:windows -entry:mainCRTStartup")
endfunction()

# Setup Resource File
set(GFX_RES_VERSION ${GFX_VERSION})
string(REGEX REPLACE "\\." "," GFX_RES_VERSION ${GFX_RES_VERSION})
string(TIMESTAMP CURRENT_YEAR %Y UTC)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/res/app.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/res/app.rc @ONLY)

include(all_executables.cmake)