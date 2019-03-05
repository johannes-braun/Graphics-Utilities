add_executable(cloud main.cpp input/input_glfw.cpp)

target_link_libraries(cloud PRIVATE gfx_ext::glfw)
target_link_libraries(cloud PRIVATE gfx_ext::mygl)

find_package(OpenMP REQUIRED)
target_link_libraries(cloud PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(cloud PRIVATE jbraun::gfx.core)
target_link_libraries(cloud PRIVATE jbraun::gfx.math)
target_link_libraries(cloud PRIVATE jbraun::gfx.ecs)
target_link_libraries(cloud PRIVATE jbraun::gfx.ecs.components)
