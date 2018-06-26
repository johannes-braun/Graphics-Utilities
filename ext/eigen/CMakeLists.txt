cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(Eigen)
add_library(eigen INTERFACE)
target_include_directories(eigen INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
install(TARGETS eigen EXPORT eigenTargets
    INCLUDES include)

install(EXPORT eigenTargets
    FILE eigenTargets.cmake
    NAMESPACE eigen::
    DESTINATION lib/cmake/eigen
)

install(DIRECTORY include/Eigen/src DESTINATION include/Eigen COMPONENT Devel FILES_MATCHING PATTERN "*.h")

file(GLOB Eigen_H Eigen/*)
install(FILES ${Eigen_H} DESTINATION include)