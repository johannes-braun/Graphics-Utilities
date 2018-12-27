set(current_executable file)
mark_as_executable(${current_executable})

target_link_libraries(${current_executable} PRIVATE jbraun::gfx.core)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.math)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.ecs)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.ecs.defaults2)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.file)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.graphics)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.shaders)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/relative.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/relative.hpp @ONLY)