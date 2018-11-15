include(${CMAKE_MODULE_PATH}/spv/compile_spv.cmake)

create_spirv(gfx.shaders ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include/gfx.shaders/shaders.hpp core)
target_include_directories(gfx.shaders INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/)
target_include_directories(gfx.shaders INTERFACE $<INSTALL_INTERFACE:include>)
add_library(jbraun::gfx.shaders ALIAS gfx.shaders)