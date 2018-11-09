add_executable(AMR_assignment02 main.cpp MatrixIO.cpp Eigen.natvis SlamGraph.cpp)
target_link_libraries(AMR_assignment02 PRIVATE gfx.math gfx.file)