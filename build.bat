@echo off

set "WORKSPACE_DIR=%~dp0"
if "%WORKSPACE_DIR:~-1%"=="\" set "WORKSPACE_DIR=%WORKSPACE_DIR:~0,-1%"

set "INCLUDES= -I %WORKSPACE_DIR% -I %WORKSPACE_DIR%/include "
set "OPTIONS= /std:c++20 -DUNICODE -D_UNICODE -Zi /GF /GR"
set "BUILD_DIR=%WORKSPACE_DIR%\build"

if not exist %BUILD_DIR% mkdir %BUILD_DIR%
pushd build

cl %OPTIONS% %INCLUDES% %WORKSPACE_DIR%/src/main.cpp


popd