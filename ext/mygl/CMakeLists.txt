set(mygl_headers 
    ${CMAKE_CURRENT_SOURCE_DIR}/mygl/mygl.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mygl/mygl_enums.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mygl/mygl_extensions.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mygl/mygl_functions.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mygl/mygl_functions.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/mygl/mygl_loader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mygl/mygl_loader.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/mygl/mygl_types.hpp)

add_library(mygl INTERFACE)
target_include_directories(mygl INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/mygl>
)
target_compile_features(mygl INTERFACE cxx_std_17)
install(TARGETS mygl EXPORT myglTargets
    INCLUDES DESTINATION include)

install(EXPORT myglTargets
    FILE myglTargets.cmake
    NAMESPACE jbraun::
    DESTINATION lib/cmake/mygl
)
install(FILES ${mygl_headers} DESTINATION include/mygl)

add_library(jbraun::mygl ALIAS mygl)