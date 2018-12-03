
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_BINARY_DIR}>)

# Find the QtWidgets library
#set(CMAKE_PREFIX_PATH $ENV{QT5_ROOT})
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
if(NOT Qt5Widgets_FOUND)
	gfx_msg("Qt5 not found. Compiling without Qt5.")
	set(GFX_HAS_QT5 FALSE)
else()
	set(GFX_HAS_QT5 TRUE)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTORCC ON)
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(GFX_APP_RESOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/res/)
if(WIN32)
    set(GFX_APP_RESOURCES ${GFX_APP_RESOURCE_PATH}/app.rc)
endif()

function(copy_dll target lib)
	add_custom_command(
		TARGET ${target}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
			$<TARGET_FILE:${lib}>
			$<TARGET_FILE_DIR:${target}>/$<TARGET_FILE_NAME:${lib}>
		)
endfunction()

function(mark_as_executable exe_name)
    file(GLOB_RECURSE CPPS *.cpp *.natvis)

	if(Qt5Widgets_FOUND)
		file(GLOB UIS *.ui)
		file(GLOB MOCS *.moc.h)
		file(GLOB RESS *.qrc)
		
		list(APPEND CPPS ${RES_SOURCES} ${UI_HEADERS} ${MOC_SRCS})
	endif()
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