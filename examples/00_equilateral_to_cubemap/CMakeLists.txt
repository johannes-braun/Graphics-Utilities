if(NOT GFX_HAS_QT5)
	message(FATAL_ERROR "Cannot build this sample without Qt.")
endif()

set(current_executable 00_equilateral_to_cubemap)

add_executable(${current_executable} main.cpp ${GFX_APP_RESOURCES})
target_compile_features(${current_executable} PRIVATE cxx_std_17)
target_link_libraries(${current_executable} PRIVATE jbraun::gfx.file Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Concurrent)

copy_dll(${current_executable} Qt5::Core)
copy_dll(${current_executable} Qt5::Widgets)
copy_dll(${current_executable} Qt5::Gui)
copy_dll(${current_executable} Qt5::Concurrent)