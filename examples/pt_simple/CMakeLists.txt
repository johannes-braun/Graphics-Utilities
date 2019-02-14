add_executable(pt_simple main.cpp input/input_glfw.cpp)

find_package(glfw3 REQUIRED)
target_link_libraries(pt_simple PRIVATE glfw)
target_link_libraries(pt_simple PRIVATE gfx_ext::mygl)

find_package(OpenMP REQUIRED)
target_link_libraries(pt_simple PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(pt_simple PRIVATE jbraun::gfx.core)
target_link_libraries(pt_simple PRIVATE jbraun::gfx.math)
target_link_libraries(pt_simple PRIVATE jbraun::gfx.ecs)
target_link_libraries(pt_simple PRIVATE jbraun::gfx.ecs.components)

target_link_libraries(pt_simple PRIVATE gfx_ext::qt5_core)
target_link_libraries(pt_simple PRIVATE gfx_ext::qt5_widgets)
target_link_libraries(pt_simple PRIVATE gfx_ext::qt5_gui)
target_compile_definitions(pt_simple PRIVATE QT_NO_OPENGL)
