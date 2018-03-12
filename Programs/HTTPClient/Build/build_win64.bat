@echo off
REM
REM
%~d0
cd %~dp0
cd ../
mkdir projects
cd projects
mkdir proj.win64
cd proj.win64
cmake -G "Visual Studio 14 2015 Win64" ../../
cmake --build . --target INSTALL --config Debug
cmake --build . --target INSTALL --config Release
pause