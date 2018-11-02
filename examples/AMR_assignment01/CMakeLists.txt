add_executable(AMR_assignment01 main.cpp MatrixIO.cpp Eigen.natvis)
target_link_libraries(AMR_assignment01 PRIVATE gfx.math gfx.file)