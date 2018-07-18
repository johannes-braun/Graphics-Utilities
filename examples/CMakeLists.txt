function(mark_as_executable exe_name)
    file(GLOB CPPS ${exe_name}/*.cpp)
    add_executable(${exe_name} ${CPPS})
    if(WIN32)
        target_sources(${exe_name} PUBLIC res/app.rc)
    endif()
    target_compile_features(${exe_name} PRIVATE cxx_std_17)
    target_include_directories(${exe_name} PUBLIC
        $<BUILD_INTERFACE:${var_include_dir}>
        $<INSTALL_INTERFACE:include>
    )
    install(TARGETS ${exe_name} EXPORT ${exe_name}Targets
        RUNTIME DESTINATION bin
    )
    target_link_libraries(${exe_name} jbraun::gfx)
    
    target_compile_options(${exe_name} PRIVATE ${compile_options})
    set_target_properties(${exe_name} PROPERTIES LINK_FLAGS_RELEASE "-subsystem:windows -entry:mainCRTStartup")

    add_custom_command(
        TARGET ${exe_name} 
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:assimp>
            $<TARGET_FILE_DIR:${exe_name}>/$<TARGET_FILE_NAME:assimp>
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:glfw>
            $<TARGET_FILE_DIR:${exe_name}>/$<TARGET_FILE_NAME:glfw>
    )

	if(TARGET shaders_${exe_name})
		add_dependencies(${exe_name} shaders_${exe_name})
	endif()
endfunction()

set(GFX_RES_VERSION ${GFX_VERSION})
string(REGEX REPLACE "\\." "," GFX_RES_VERSION ${GFX_RES_VERSION})
string(TIMESTAMP CURRENT_YEAR %Y UTC)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/res/app.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/res/app.rc @ONLY)

mark_as_executable(00_equilateral_to_cubemap)
mark_as_executable(01_color_normalization)
mark_as_executable(02_forward_renderer)
mark_as_executable(03_bidirectional_pathtracer)
mark_as_executable(04_marching_squares)
mark_as_executable(05_implementation_tests)
mark_as_executable(06_sail_simulation)
mark_as_executable(07_meshing)
mark_as_executable(08_point_detection)
mark_as_executable(09_bv_stuff)
mark_as_executable(10_ecs)
mark_as_executable(11_delaunay)
mark_as_executable(12_bezier)
mark_as_executable(13_ibl)