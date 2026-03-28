#!/usr/bin/env bash
set -euo pipefail



# Config
BUILD_DIR=${BUILD_DIR:-build}
BUILD_TYPE=${BUILD_TYPE:-Debug}
NUM_JOBS=$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 8)
CMAKE_GENERATOR=${CMAKE_GENERATOR:-}
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"



# Parse Arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        release|--release) BUILD_TYPE=Release ;;
        debug|--debug)     BUILD_TYPE=Debug ;;
        ninja|--ninja)     CMAKE_GENERATOR="-G Ninja" ;;
        clean|--clean)     rm -rf "$BUILD_DIR"; echo "Cleaned $BUILD_DIR"; exit 0 ;;
        *) echo "Unknown argument: $1"; exit 1 ;;
    esac
    shift
done



# Create and enter build direcotry
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"



# Configure CMake
echo "Configuring ($BUILD_TYPE) in $BUILD_DIR ..."
cmake ../cmake \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    $CMAKE_GENERATOR \
    "${CMAKE_ARGS:-}"  # allow CMAKE_ARGS="..." ./compile.sh for extra flags



# Refresh compile_commands.json
rm -f ../compile_commands.json
if [[ -f compile_commands.json ]]; then
    ln -sf "$(pwd)/compile_commands.json" ../compile_commands.json
    chmod a-x ../compile_commands.json 2>/dev/null || true
    echo "compile_commands.json linked"
fi



# ----------- Build -----------------------------------------------------
echo "Building with $NUM_JOBS jobs ..."
cmake --build . -- -j"$NUM_JOBS"


