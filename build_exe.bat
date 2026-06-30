@echo off
:: build_exe.bat -- Configure (if needed) and build only the daisy-bin executable.
::
:: Requires MSYS2 ucrt64 installed at C:\msys64.
:: Uses the CMake preset "mingw-gcc-native-local" defined in CMakeUserPresets.json.

set CMAKE=C:\msys64\ucrt64\bin\cmake.exe
set SOURCE_DIR=%~dp0
set BUILD_DIR=%~dp0build\release

if not exist "%CMAKE%" (
    echo ERROR: cmake not found at %CMAKE%
    echo Please install MSYS2 and run: pacman -S mingw-w64-ucrt-x86_64-cmake
    exit /b 1
)

:: MSYS2 ucrt64 must be on PATH so cmake can find ninja.exe and g++.exe
set PATH=C:\msys64\ucrt64\bin;%PATH%

:: Configure (or reconfigure) using the local preset.
:: The preset defines both source and build dirs, so just cd to the source root.
echo Configuring...
cd /d "%~dp0"
"%CMAKE%" --preset mingw-gcc-native-local
if %ERRORLEVEL% neq 0 (
    echo CONFIGURE FAILED
    exit /b %ERRORLEVEL%
)

echo Building daisy-bin...
"%CMAKE%" --build "%BUILD_DIR%" --target daisy-bin -j%NUMBER_OF_PROCESSORS%

if %ERRORLEVEL% neq 0 (
    echo BUILD FAILED
    exit /b %ERRORLEVEL%
)

echo.
echo Build succeeded: %BUILD_DIR%\daisy-bin.exe
