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