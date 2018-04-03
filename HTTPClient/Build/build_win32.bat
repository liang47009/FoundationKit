@echo off
REM
REM
%~d0
cd %~dp0
cd ../
mkdir projects
cd projects
mkdir proj.nmake
cd proj.nmake

mkdir Debug
mkdir Release

cd Debug
cmake -G "NMake Makefiles"  -DCMAKE_BUILD_TYPE=Debug ../../../
:nmake

cd ../Release
cmake -G "NMake Makefiles"  -DCMAKE_BUILD_TYPE=Release ../../../
:nmake

pause