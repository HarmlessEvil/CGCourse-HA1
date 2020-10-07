@ECHO ON

SET DEBUG=false
SET DEBUG_DIRECTORY=build-debug

REM arguments parsing
REM Discards all unknown arguments without being able to
REM restore it
:while
IF NOT "%1"=="" (
    IF "%1"=="--debug" (
        SET DEBUG=true
        SHIFT
    )
    GOTO :while
)

SET BUILD_DIRECTORY=build
SET CONAN_FLAGS=--build missing
SET DCMAKE_BUILD_TYPE=Release

IF "%DEBUG%"=="true" (
    SET BUILD_DIRECTORY=%DEBUG_DIRECTORY%
    SET CONAN_FLAGS=%CONAN_FLAGS% -s build_type=Debug
    SET DCMAKE_BUILD_TYPE=Debug
)

RMDIR /Q /S build
MKDIR build
PUSHD build

conan install .. %CONAN_FLAGS%
cmake .. -G "MinGW Makefiles"
cmake --build . --config %DCMAKE_BUILD_TYPE%
cmake --install .
