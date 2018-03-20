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



configure() {
    local api_level=$1
    local abi=$2
    local build_type=$3
    cmake \
      -DCMAKE_TOOLCHAIN_FILE=../../../../CMake/android.toolchain.wapper.cmake \
      -DANDROID_NATIVE_API_LEVEL=$api_level                           \
      -DANDROID_ABI=$abi                                              \
      -DANDROID_STL=c++_static                                        \
      -DANDROID_STL_FORCE_FEATURES=ON                                 \
      -DCMAKE_BUILD_TYPE=$build_type                                  \
      ../../../../                                                    
}

build() {
    cmake --build . -- -j8 -O
}

configure_and_build() {
    local dir=$1
    shift
    mkdir -p "$dir"
    if ! ( cd "$dir" &&
	   configure "$@" &&
	   build ); then
	echo "Failed to compile $*"
	exit 1
    fi
}

#                   directory           abi level  abi         build type
configure_and_build debug/armeabi-v7a   android-23 armeabi-v7a Debug
configure_and_build debug/arm64-v8a     android-23 arm64-v8a   Debug
configure_and_build release/armeabi-v7a android-23 armeabi-v7a Release
configure_and_build release/arm64-v8a   android-23 arm64-v8a   Release
