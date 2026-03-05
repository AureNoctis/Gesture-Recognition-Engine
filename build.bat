@echo off

set "WORKSPACE_DIR=%~dp0"
if "%WORKSPACE_DIR:~-1%"=="\" set "WORKSPACE_DIR=%WORKSPACE_DIR:~0,-1%"

mkdir build
pushd build

cl /std:c++20 -DUNICODE -D_UNICODE -Zi /EHsc %WORKSPACE_DIR%\code\main.cpp


popd