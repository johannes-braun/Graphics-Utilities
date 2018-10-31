include(${CMAKE_MODULE_PATH}/spv/compile_spv.cmake)

create_spirv(gfx.shaders ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include/gfx.shaders/path.hpp core)
