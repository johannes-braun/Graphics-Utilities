set(current_executable 06_descriptors)
mark_as_executable(${current_executable})

include(${CMAKE_MODULE_PATH}/spv/compile_spv.cmake)
create_spirv(${current_executable}.shaders ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shaders.hpp spectral)

target_link_libraries(${current_executable} PRIVATE ${current_executable}.shaders)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.core)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.math)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.ecs)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.ecs.defaults)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.file)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.legacy)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.shaders)
target_link_libraries(${current_executable} PRIVATE Qt5::Core)
target_link_libraries(${current_executable} PRIVATE Qt5::Widgets)
target_link_libraries(${current_executable} PRIVATE Qt5::Gui)
target_link_libraries(${current_executable} PRIVATE Qt5::Concurrent)
copy_dll(${current_executable} assimp::assimp)
copy_dll(${current_executable} Qt5::Core)
copy_dll(${current_executable} Qt5::Widgets)
copy_dll(${current_executable} Qt5::Gui)
copy_dll(${current_executable} Qt5::Concurrent)

# Needed for Ninja :(
set_target_properties(${current_executable} PROPERTIES LINK_FLAGS 
	"-natvis:${CMAKE_CURRENT_SOURCE_DIR}/graphics/graphics.natvis")