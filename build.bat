@ECHO ON

RMDIR /Q /S build
MKDIR build
PUSHD build

conan install .. --build missing
cmake .. -G "Visual Studio 15 2017 Win64"
cmake --build . --config Release
