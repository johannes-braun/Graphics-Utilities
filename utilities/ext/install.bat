@echo off

where git | REM
if errorlevel == 1 (
    echo [Error] Failed to find git.
    goto :eof
)

where cmake | REM
if errorlevel == 1 (
    echo [Error] Failed to find cmake.
    goto :eof
)
SET GEN="Visual Studio 15 2017 Win64"

set repo="https://github.com/zeux/pugixml.git"
set cmake_flags=
call :buildlib

set repo="https://gitlab.uni-koblenz.de/johannesbraun/glshader.git"
set cmake_flags=
call :buildlib

set repo="https://github.com/glfw/glfw.git"
set cmake_flags=-DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_EXAMPLES=OFF
call :buildlib

set repo="https://github.com/kcat/openal-soft.git"
set cmake_flags=-DALSOFT_UTILS=OFF -DALSOFT_EXAMPLES=OFF -DALSOFT_TESTS=OFF
call :buildlib

set repo="https://github.com/assimp/assimp.git"
set cmake_flags=-DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_BUILD_TESTS=OFF
call :buildlib

goto :eof

:buildlib
setlocal
FOR /F "delims=" %%i IN ("%repo%") DO (
    set libname=%%~ni
)
if exist installed\%libname%\ (
    echo %libname% already exists.
) else (
    echo Cloning %libname% repo...
    git clone %repo% git\%libname%
    
    mkdir git\%libname%\build
    cd git\%libname%\build

    echo Building %libname%:Debug...
    cmake .. %cmake_flags% -G%GEN% -DCMAKE_INSTALL_PREFIX:PATH=..\..\..\installed\%libname%\Release\
    cmake --build . --target install --config Release

    echo Building %libname%:Release...
    cmake .. %cmake_flags% -G%GEN% -DCMAKE_INSTALL_PREFIX:PATH=..\..\..\installed\%libname%\Debug\
    cmake --build . --target install --config Debug

    cd ..\..\..
    rd /s /q git
    echo -----------------------------------------
)
