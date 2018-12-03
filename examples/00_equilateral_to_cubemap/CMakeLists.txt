set(current_executable 00_equilateral_to_cubemap)
mark_as_executable(${current_executable})

target_link_libraries(${current_executable} PRIVATE jbraun::gfx.file)

target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_core)
target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_widgets)
target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_gui)
target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_concurrent)
