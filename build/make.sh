#!/bin/bash

OPENDSE_ROOT=$(pwd)/..
CC="gcc"
CC_FLAGS="-Wall -Wextra -O2"
LD_FLAGS=""
OPENDSE_DEBUG=false
OPENDSE_LEGACY=false

show_help() {
    cat << EOF
Open Digital Sound Engine (OpenDSE) Build Script

Usage: $0 [options]
    
Help options:
    --help                    print this message

Standard options:
    --enable-debug            build and include debug symbols to binaries
    --enable-legacy-support   enable OSS legacy support for Linux/BSD (not implemented yet)
    --disable-logging         do not log configure debug information
    --prefix                  install library into specified path
EOF
}

while [ $# -gt 0 ]; do
    case $1 in
      --help)
          show_help
          exit 0
          ;;
      --enable-debug)
          OPENDSE_DEBUG=true
          shift
          ;;
      --enable-legacy-support)
          OPENDSE_LEGACY=true
          shift
          ;;
      --prefix)
          PREFIX="$2"
          shift 2
          ;;
      *)
          echo "Unknown parameter '$1'"
          exit 1
          ;;
    esac
done

makeOpenDSE() {
    echo "[1/4] Cleaning previous builds..."
    rm -rf ../out

    echo "[2/3] Building OpenDSE from source..."

    cd $OPENDSE_ROOT/proj/gcc
    make OPENDSE_DEBUG=$OPENDSE_DEBUG OPENDSE_LEGACY=$OPENDSE_LEGACY
    cd $OPENDSE_ROOT

    echo "[3/3] Installing OpenDSE... (requires su permissions)"

    su -c "cp out/bin/libopendse.so $PREFIX/libopendse.so"

    echo Done!
    echo 
}

makeOpenDSE