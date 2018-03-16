@echo off
REM
REM
%~d0
cd %~dp0
cd ../
mkdir projects
cd projects
mkdir proj.android
cd proj.android

mkdir objs
cd objs
mkdir armeabi
mkdir armeabi-v7a
mkdir x86
cd ../

cd objs\armeabi
cmake -G "Unix Makefiles" ^
-DCMAKE_TOOLCHAIN_FILE=%~dp0../CMake/android.toolchain.wapper.cmake ^
-DCMAKE_BUILD_TYPE=Release ^
-DANDROID_ABI="armeabi" ^
../../../
cmake --build . --target install 


cd ..\..\objs\armeabi-v7a
cmake -G "Unix Makefiles" ^
-DCMAKE_TOOLCHAIN_FILE=%~dp0../CMake/android.toolchain.wapper.cmake ^
-DCMAKE_BUILD_TYPE=Release ^
-DANDROID_ABI="armeabi-v7a" ^
../../../
cmake --build . --target install 

cd ..\..\objs\x86
cmake -G "Unix Makefiles" ^
-DCMAKE_TOOLCHAIN_FILE=%~dp0../CMake/android.toolchain.wapper.cmake ^
-DCMAKE_BUILD_TYPE=Release ^
-DANDROID_ABI="x86" ^
../../../
cmake --build . --target install 

pause