#!/bin/bash

DEBUG=false
DEBUG_DIRECTORY=build-debug

# arguments parsing
POSITIONAL=()
while [[ $# -gt 0 ]]
do
  case "$1" in
    --debug)
      DEBUG=true
      shift
      ;;
    *) # unknown argument
      POSITIONAL+=("$1")
      shift
      ;;
  esac
done

set -- "${POSITIONAL[@]}" # restore unknown arguments

# Defaults
BUILD_DIRECTORY=build
CONAN_FLAGS=()
DCMAKE_BUILD_TYPE=Release

if [[ $DEBUG = true ]]
then
  BUILD_DIRECTORY="$DEBUG_DIRECTORY"
  CONAN_FLAGS+=(-s "build_type=Debug")
  DCMAKE_BUILD_TYPE=Debug
fi

set -e
set -x

rm -rf "$BUILD_DIRECTORY"
mkdir "$BUILD_DIRECTORY"
pushd "$BUILD_DIRECTORY"

export CONAN_SYSREQUIRES_MODE=enabled
conan install .. "${CONAN_FLAGS[@]}"
cmake .. -DCMAKE_BUILD_TYPE="$DCMAKE_BUILD_TYPE"
cmake --build .
cmake --install .
