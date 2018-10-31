

# Find the QtWidgets library
set(CMAKE_PREFIX_PATH $ENV{QT5_ROOT})
find_package(Qt5Widgets)
find_package(Qt5Concurrent)
if(NOT Qt5Widgets_FOUND)
	gfx_msg("Qt5 not found. Compiling without Qt5.")
else()
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTORCC ON)
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

function(copy_dll target lib)
	add_custom_command(
		TARGET ${target}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
			$<TARGET_FILE:${lib}>
			$<TARGET_FILE_DIR:${exe_name}>/$<TARGET_FILE_NAME:${lib}>
		)
endfunction()

set(app_res_path ${CMAKE_CURRENT_SOURCE_DIR}/res/)
function(mark_as_executable exe_name)
    file(GLOB_RECURSE CPPS *.cpp *.natvis)

	if(Qt5Widgets_FOUND)
		file(GLOB UIS *.ui)
		file(GLOB MOCS *.moc.h)
		file(GLOB RESS *.qrc)
		
		list(APPEND CPPS ${RES_SOURCES} ${UI_HEADERS} ${MOC_SRCS})
	endif()
	add_executable(${exe_name} ${CPPS})

    if(WIN32)
        target_sources(${exe_name} PUBLIC ${app_res_path}/app.rc)
    endif()
    target_compile_features(${exe_name} PRIVATE cxx_std_17)
    target_include_directories(${exe_name} PUBLIC
        $<BUILD_INTERFACE:${var_include_dir}>
        $<INSTALL_INTERFACE:include>
    )
	if(GFX_USE_INSTALL)
		install(TARGETS ${exe_name} EXPORT ${exe_name}Targets
			RUNTIME DESTINATION bin
		)
	endif()
    target_link_libraries(${exe_name} PRIVATE jbraun::gfx.core jbraun::gfx.math jbraun::gfx.ecs jbraun::gfx.ecs.defaults jbraun::gfx.file jbraun::gfx.legacy gfx.shaders
		)
	if(WIN32)
		target_link_libraries(${exe_name} PRIVATE comctl32)
	endif()
    
    target_compile_options(${exe_name} PRIVATE ${compile_options})
    set_target_properties(${exe_name} PROPERTIES LINK_FLAGS_RELEASE "-subsystem:windows -entry:mainCRTStartup")
	
	copy_dll(${exe_name} assimp)
	copy_dll(${exe_name} glfw)
	
	if(Qt5Widgets_FOUND)
		target_link_libraries(${exe_name}
			PUBLIC Qt5::Widgets Qt5::Concurrent)
		
		copy_dll(${exe_name} Qt5::Core)
		copy_dll(${exe_name} Qt5::Widgets)
		copy_dll(${exe_name} Qt5::Gui)
		copy_dll(${exe_name} Qt5::Concurrent)
	endif()

#	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../src/shaders/${exe_name}/CMakeLists.txt)
		#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../src/shaders/${exe_name} ../src/shaders/${exe_name})
		#add_dependencies(${exe_name} shaders_${exe_name})
	#endif()
endfunction()

set(GFX_RES_VERSION ${GFX_VERSION})
string(REGEX REPLACE "\\." "," GFX_RES_VERSION ${GFX_RES_VERSION})
string(TIMESTAMP CURRENT_YEAR %Y UTC)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/res/app.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/res/app.rc @ONLY)

mark_as_executable(00_equilateral_to_cubemap)
add_subdirectory(06_descriptors)
#[[
mark_as_executable(00_equilateral_to_cubemap)
mark_as_executable(01_color_normalization)
mark_as_executable(02_sound)
mark_as_executable(03_ai)
mark_as_executable(04_marching_squares)
mark_as_executable(05_qt)
mark_as_executable(07_meshing)
mark_as_executable(08_point_detection)
mark_as_executable(09_bv_stuff)
mark_as_executable(10_ecs)
mark_as_executable(11_pathtracer_incremental)
mark_as_executable(12_boids)
mark_as_executable(13_ibl)
mark_as_executable(14_cubemapper)
mark_as_executable(15_win32_window)
mark_as_executable(17_proto)
]]