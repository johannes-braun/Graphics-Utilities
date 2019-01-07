set(current_executable vk_and_gl)
mark_as_executable(${current_executable})

include(${CMAKE_MODULE_PATH}/spv/compile_spv.cmake)
create_spirv(${current_executable}.shaders ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shaders.cpp vkgl)

add_library(${current_executable}-shaders SHARED ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shaders.cpp)
target_link_libraries(${current_executable}-shaders PRIVATE ${current_executable}.shaders)
set_target_properties(${current_executable}-shaders PROPERTIES 
	RUNTIME_OUTPUT_NAME shaders
	RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:${current_executable}>)

#target_link_libraries(${current_executable} PRIVATE ${current_executable}.shaders)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.core)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.math)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.ecs)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.ecs.defaults2)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.file)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.graphics)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.shaders)

target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_core)
target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_widgets)
target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_gui)
target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_concurrent)
target_link_libraries(${current_executable} PRIVATE gfx_ext::qt5_charts)
target_link_libraries(${current_executable} PRIVATE gfx_ext::glfw)
target_link_libraries(${current_executable} PRIVATE gfx_ext::mygl)