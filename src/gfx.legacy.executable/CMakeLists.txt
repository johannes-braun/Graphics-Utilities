set(current_target gfx.legacy.executable)
add_library(${current_target} INTERFACE)
target_sources(${current_target} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/executable.cpp)
target_include_directories(${current_target} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/)

target_link_libraries(${current_target} INTERFACE jbraun::gfx.legacy)
target_link_libraries(${current_target} INTERFACE jbraun::gfx.ecs)
target_link_libraries(${current_target} INTERFACE jbraun::gfx.ecs.defaults)

add_library(jbraun::gfx.legacy.executable ALIAS gfx.legacy.executable)