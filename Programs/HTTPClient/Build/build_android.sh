# !/bin/bash
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

cd armeabi
cmake -G "Unix Makefiles" \
-DCMAKE_TOOLCHAIN_FILE=../../../../CMake/android.toolchain.wapper.cmake \
-DCMAKE_BUILD_TYPE=Release \
-DANDROID_ABI="armeabi" \
../../../../
make
make install


cd ../armeabi-v7a
cmake -G "Unix Makefiles" \
-DCMAKE_TOOLCHAIN_FILE=../../../../CMake/android.toolchain.wapper.cmake \
-DCMAKE_BUILD_TYPE=Release \
-DANDROID_ABI="armeabi-v7a" \
../../../../
make
make install

cd ../x86
cmake -G "Unix Makefiles" \
-DCMAKE_TOOLCHAIN_FILE=../../../../CMake/android.toolchain.wapper.cmake \
-DCMAKE_BUILD_TYPE=Release \
-DANDROID_ABI="x86" \
../../../../
make
make install
