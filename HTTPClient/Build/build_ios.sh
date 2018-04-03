# !/bin/bash
cd ../
mkdir projects
cd projects
mkdir proj.ios
cd proj.ios
cmake -DCMAKE_TOOLCHAIN_FILE=../../CMake/ToolChains/ios.toolchain.cmake -DIOS_PLATFORM=OS -DIOS_BITCODE=FALSE ../../ 
make
make install