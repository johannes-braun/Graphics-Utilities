find_library(LIBGLFW NAMES libglfw3.a HINTS ${AS_EXTERN_LIBRARIES_PATH}/glfw/linux/)
find_library(LIBASSIMP NAMES libassimp.so.3)
find_library(LIBX11 NAMES X11)
find_library(LIBXRANDR NAMES Xrandr)
find_library(LIBXINERAMA NAMES Xinerama)
find_library(LIBXXF86VM NAMES Xxf86vm)
find_library(LIBXI NAMES Xi)
find_library(LIBPTHREAD NAMES pthread)
find_library(LIBXCURSOR NAMES Xcursor)
find_library(LIBDL NAMES dl)
find_library(LIBGL NAMES GL)

list(APPEND LIBRARIES
    ${LIBGLFW}
    ${LIBASSIMP}
    ${LIBX11}
    ${LIBXRANDR}
    ${LIBXINERAMA}
    ${LIBXXF86VM}
    ${LIBXI}
    ${LIBPTHREAD}
    ${LIBXCURSOR}
    ${LIBDL}
    ${LIBGL}
    stdc++fs
)