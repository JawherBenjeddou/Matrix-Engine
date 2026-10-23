@echo off
rem Fetches dependencies, generates Visual Studio project files and builds the Debug configuration.
rem Pass "Release" as the first argument to build the Release configuration instead.
setlocal
set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=Debug
python "%~dp0scripts\setup.py" --build --config %CONFIG%
endlocal
