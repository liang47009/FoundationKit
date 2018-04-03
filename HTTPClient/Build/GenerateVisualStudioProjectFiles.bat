@echo off
REM
REM
%~d0
cd %~dp0
cd ../
mkdir projects
cd projects
mkdir proj.win32
cd proj.win32
cmake -G "Visual Studio 14 2015" ../../
:cmake -G "Visual Studio 14 2015 Win64" ../../
:cmake --build . --config Debug
:cmake --build . --config Release
pause