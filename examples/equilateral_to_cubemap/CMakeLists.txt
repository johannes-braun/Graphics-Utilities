set(current_executable 00_equilateral_to_cubemap)

add_executable(${current_executable} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.default_app)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.file)
target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_core)
target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_widgets)
target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_gui)
target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_concurrent)
