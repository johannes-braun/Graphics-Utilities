mark_as_executable(06_descriptors)
include(${CMAKE_MODULE_PATH}/spv/compile_spv.cmake)
create_spirv(06_descriptors.shaders ${CMAKE_CURRENT_SOURCE_DIR}/shaders ${CMAKE_CURRENT_SOURCE_DIR}/shaders/path.hpp spectral)
target_link_libraries(06_descriptors PRIVATE 06_descriptors.shaders)