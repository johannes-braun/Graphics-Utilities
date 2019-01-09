set(current_executable vk_and_gl)
mark_as_executable(${current_executable})

include(spv/spv_to_dll)
add_shader(${current_executable}.shaders_vk vulkan
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/vk_vs.vert
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/vk_fs.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/vk_vs_shadow.vert)
	
add_shader(${current_executable}.shaders_gl opengl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/gl_vs.vert
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/gl_fs.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/gl_vs_shadow.vert)

set_target_properties(${current_executable}.shaders_vk PROPERTIES 
	RUNTIME_OUTPUT_NAME shaders_vk
	RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:${current_executable}>)
set_target_properties(${current_executable}.shaders_gl PROPERTIES 
	RUNTIME_OUTPUT_NAME shaders_gl
	RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:${current_executable}>)

add_dependencies(${current_executable} ${current_executable}.shaders_vk ${current_executable}.shaders_gl)
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

target_compile_definitions(${current_executable} PRIVATE QT_NO_OPENGL)
find_package(OpenMP REQUIRED)
target_link_libraries(${current_executable} PRIVATE OpenMP::OpenMP_CXX)
