@echo off

set GAME_NAME=PureLogicSolver.exe
set RAYLIB_SRC=C:\raylib\raylib\src
set SOURCES=PureLogicSolver.c

setlocal EnableDelayedExpansion
setlocal EnableExtensions

set "ROOT_DIR=%CD%"
set "SOURCES=!ROOT_DIR!\!SOURCES!"
set OUTPUT_DIR=build
set "RESOURCES=!ROOT_DIR!\resources"

echo copying resources
xcopy /s/i/D/Y !RESOURCES! !OUTPUT_DIR!\resources

set OUTPUT_FLAG=/Fe: "!GAME_NAME!"
set COMPILATION_FLAGS=/std:c11 /Od /Zi /utf-8 /validate-charset /EHsc
set WARNING_FLAGS=/W3 /sdl
set SUBSYSTEM_FLAGS=/DEBUG
set LINK_FLAGS=/link kernel32.lib user32.lib shell32.lib winmm.lib gdi32.lib opengl32.lib
set "TEMP_DIR=temp"

IF NOT EXIST !TEMP_DIR!\ (
    echo Compiling Raylib in temp
    mkdir !TEMP_DIR!
    cd !TEMP_DIR!
    set "RAYLIB_DEFINES=/D_DEFAULT_SOURCE /DPLATFORM_DESKTOP /DGRAPHICS_API_OPENGL_33"
    set RAYLIB_C_FILES="!RAYLIB_SRC!\rcore.c" "!RAYLIB_SRC!\rshapes.c" "!RAYLIB_SRC!\rtextures.c" "!RAYLIB_SRC!\rtext.c" "!RAYLIB_SRC!\rmodels.c" "!RAYLIB_SRC!\utils.c" "!RAYLIB_SRC!\raudio.c" "!RAYLIB_SRC!\rglfw.c"
    set RAYLIB_INCLUDE_FLAGS=/I"!RAYLIB_SRC!" /I"!RAYLIB_SRC!\external\glfw\include"

    cl.exe /w /c !RAYLIB_DEFINES! !RAYLIB_INCLUDE_FLAGS! !COMPILATION_FLAGS! !RAYLIB_C_FILES! || exit /B
    cd !ROOT_DIR!
)

IF NOT EXIST !OUTPUT_DIR! mkdir !OUTPUT_DIR!
cd !OUTPUT_DIR!

echo compiling .exe
cl.exe !COMPILATION_FLAGS! !WARNING_FLAGS! /c /I"!RAYLIB_SRC!" !SOURCES! || exit /B
cl.exe !OUTPUT_FLAG! "!ROOT_DIR!\!TEMP_DIR!\*.obj" *.obj !LINK_FLAGS! !SUBSYSTEM_FLAGS! || exit /B

echo finished copiling

echo run game:
!GAME_NAME!

cd !ROOT_DIR!
