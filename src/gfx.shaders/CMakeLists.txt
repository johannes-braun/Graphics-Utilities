include(${CMAKE_MODULE_PATH}/spv/compile_spv.cmake)

create_spirv(gfx.shaders ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include/gfx.shaders/shaders.hpp core)
add_library(jbraun::gfx.shaders ALIAS gfx.shaders)