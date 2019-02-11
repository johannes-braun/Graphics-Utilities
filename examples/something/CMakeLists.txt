add_executable(something main.cpp)

find_package(glfw3 REQUIRED)
target_link_libraries(something PRIVATE glfw)
target_link_libraries(something PRIVATE gfx_ext::mygl)

find_package(OpenMP REQUIRED)
target_link_libraries(something PRIVATE OpenMP::OpenMP_CXX)
