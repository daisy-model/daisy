@echo off
rem Daisy Windows launcher (single-file CMD)
rem - --info       : print Daisy+Python info
rem - --pip ...    : run pip inside the bundled Python (adds --no-warn-script-location on install)
rem - --python..   : run the bundled Python
rem - --repair-pip : try to fix pip by removing any old pip and installing again
rem - default      : run daisy-bin.exe with all remaining args

setlocal

rem ----- resolve directories -----
set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%") do set "ABS_DIR=%%~fI"
for %%I in ("%SCRIPT_DIR%\..\sample") do set "SAMPLE_DIR=%%~fI"

set "DAISY=%ABS_DIR%\daisy-bin.exe"
set "PYTHONHOME=%ABS_DIR%"
set "PYTHON=%PYTHONHOME%\python.exe"

rem ----- DAISYHOME: honor env if already set -----
if defined DAISYHOME (
    set "DAISYHOME=%DAISYHOME%"
) else (
    for %%I in ("%ABS_DIR%\..") do set "DAISYHOME=%%~fI"
)

rem =======================
rem   ARGUMENT PARSING
rem =======================
set "MODE=%~1"
shift

rem First argument after MODE (used by --pip)
set "FIRST="
if not "%~1"=="" set "FIRST=%~1"

rem Collect remaining args (quoted per token)
set "ARGS="
:__argloop
if "%~1"=="" goto __after_args
set "ARGS=%ARGS% "%~1""
shift
goto __argloop
:__after_args

rem =======================
rem   DISPATCH
rem =======================
if "%MODE%"=="--info"   goto do_info
if "%MODE%"=="--pip"    goto do_pip
if "%MODE%"=="--python" goto do_python
if "%MODE%"=="--repair-pip" goto do_repairpip
goto do_default

rem =======================
rem   HANDLERS
rem =======================

rem ==========================
rem   info mode
rem ==========================
:do_info
set "PYTHONHOME=%PYTHONHOME%"
rem Show only the first line of 'daisy -v'
"%DAISY%" -v 2>&1 | findstr /n ".*" | findstr /b "1:"
if exist ".\daisy.log" del ".\daisy.log" >nul 2>&1
"%PYTHON%" --version
echo Sample dir: %SAMPLE_DIR%
exit /b %ERRORLEVEL%

rem ==========================
rem   pip mode
rem ==========================
:do_pip
set "PYTHONHOME=%PYTHONHOME%"
set "PYTHONPATH="
set "PYTHONNOUSERSITE=1"

if /i "%FIRST%"=="install" (
    "%PYTHON%" -I -m pip %ARGS% --no-warn-script-location
) else (
    if not "%ARGS%"=="" (
        "%PYTHON%" -I -m pip %ARGS%
    ) else (
        "%PYTHON%" -I -m pip
    )
)
exit /b %ERRORLEVEL%


rem ==========================
rem   python mode
rem ==========================
:do_python
"%PYTHON%" %ARGS%
exit /b %ERRORLEVEL%

:do_default
set "PYTHONHOME=%PYTHONHOME%"
set "DAISYHOME=%DAISYHOME%"

rem Reconstruct the original argv for Daisy:
if "%MODE%"=="" (
    "%DAISY%" %ARGS%
) else (
    "%DAISY%" "%MODE%" %ARGS%
)
exit /b %ERRORLEVEL%


rem ==========================
rem   pip repair mode
rem ==========================
:do_repairpip
set "PYTHONHOME=%PYTHONHOME%"
set "PYTHONPATH="
set "PYTHONNOUSERSITE=1"

echo [daisy] Repairing embedded pip...

rem Remove pip/ directory
if exist "%PYTHONHOME%\Lib\site-packages\pip" (
    echo   Removing pip package directory...
    rmdir /s /q "%PYTHONHOME%\Lib\site-packages\pip"
)

rem Remove pip-*.dist-info directories
for /d %%D in ("%PYTHONHOME%\Lib\site-packages\pip-*.dist-info") do (
    echo   Removing %%~fD ...
    rmdir /s /q "%%~fD"
)

rem Rebuild pip cleanly
echo   Reinstalling pip via ensurepip...
"%PYTHON%" -I -m ensurepip --upgrade

echo   Pip repair completed.
exit /b 0
