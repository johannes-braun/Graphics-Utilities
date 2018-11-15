set(current_executable 08_point_detection)
mark_as_executable(${current_executable})

target_link_libraries(${current_executable} PRIVATE jbraun::gfx.legacy)
