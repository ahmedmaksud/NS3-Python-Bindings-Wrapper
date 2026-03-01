#!/bin/bash
#
# Author: Ahmed Maksud; email: ahmed.maksud@email.ucr.edu
# PI: Marcelo Menezes De Carvalho; email: mmcarvalho@txstate.edu
# Texas State University
#
# setup-and-run.sh — One-shot setup, build, and run script
# ========================================================
# This script automates the complete workflow:
#   1. Renames its own directory to pb-wrapper (if needed)
#   2. Adds add_subdirectory(pb-wrapper) to the parent CMakeLists.txt (if needed)
#   3. Activates the EHRL virtual environment
#   4. Cleans, configures, and builds NS-3
#   5. Runs pb-wrapper-demo.py
#
# Usage: cd contrib/ai/examples/pb-wrapper && ./setup-and-run.sh

set -e # exit on first error

# ── configuration ──────────────────────────────────────────────
VENV_NAME="EHRL"
PYTHON_VERSION="3.11"
TARGET_DIR_NAME="pb-wrapper"
# ───────────────────────────────────────────────────────────────

# Colors
GREEN="\033[32m"
YELLOW="\033[33m"
RED="\033[31m"
CYAN="\033[36m"
RESET="\033[0m"

info() { echo -e "${GREEN}[setup] $*${RESET}"; }
warn() { echo -e "${YELLOW}[setup] $*${RESET}"; }
error() {
    echo -e "${RED}[setup] ERROR: $*${RESET}"
    exit 1
}

# ── Step 0: Locate ourselves ──────────────────────────────────
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CURRENT_DIR_NAME="$(basename "$SCRIPT_DIR")"
EXAMPLES_DIR="$(dirname "$SCRIPT_DIR")"          # contrib/ai/examples
NS3_ROOT="$(cd "$EXAMPLES_DIR/../../.." && pwd)" # ns-3.44

info "Script directory : $SCRIPT_DIR"
info "NS-3 root        : $NS3_ROOT"

# ── Step 1: Rename directory to pb-wrapper if needed ──────────
if [[ "$CURRENT_DIR_NAME" != "$TARGET_DIR_NAME" ]]; then
    NEW_DIR="$EXAMPLES_DIR/$TARGET_DIR_NAME"
    if [[ -d "$NEW_DIR" ]]; then
        error "Cannot rename: $NEW_DIR already exists."
    fi
    info "Renaming '$CURRENT_DIR_NAME' → '$TARGET_DIR_NAME' ..."
    mv "$SCRIPT_DIR" "$NEW_DIR"
    # Re-enter renamed directory
    SCRIPT_DIR="$NEW_DIR"
    cd "$SCRIPT_DIR"
    info "Renamed successfully. Now in: $(pwd)"
else
    info "Directory already named '$TARGET_DIR_NAME' — no rename needed."
fi

# ── Step 2: Add add_subdirectory(pb-wrapper) to parent CMakeLists ─
PARENT_CMAKE="$EXAMPLES_DIR/CMakeLists.txt"

if [[ ! -f "$PARENT_CMAKE" ]]; then
    error "Parent CMakeLists.txt not found at $PARENT_CMAKE"
fi

if grep -q "add_subdirectory(${TARGET_DIR_NAME})" "$PARENT_CMAKE"; then
    info "Parent CMakeLists.txt already has add_subdirectory(${TARGET_DIR_NAME})."
else
    info "Adding add_subdirectory(${TARGET_DIR_NAME}) to $PARENT_CMAKE ..."
    echo "add_subdirectory(${TARGET_DIR_NAME})" >>"$PARENT_CMAKE"
    info "Done."
fi

# ── Step 3: Activate EHRL virtual environment ─────────────────
# Venv lives at <NS3-project>/EHRL  (two levels above ns-3.44)
VENV_DIR="$(cd "$NS3_ROOT/../.." && pwd)/$VENV_NAME"

if [[ ! -f "$VENV_DIR/bin/activate" ]]; then
    error "Virtual environment not found at $VENV_DIR"
fi

info "Activating venv: $VENV_DIR"
# shellcheck disable=SC1091
source "$VENV_DIR/bin/activate"
info "Python in use: $(which python) — $(python --version)"

# ── Step 4: Clean, configure, and build NS-3 ──────────────────
cd "$NS3_ROOT"
info "Working directory: $(pwd)"

info "Running ./ns3 clean ..."
./ns3 clean

info "Running ./ns3 configure ..."
./ns3 configure --enable-examples --enable-tests -- \
    -DNS3_PYTHON_BINDINGS=ON \
    -DPython3_EXECUTABLE="../../$VENV_NAME/bin/python" \
    -DNS3_BINDINGS_INSTALL_DIR="../../$VENV_NAME/lib/python${PYTHON_VERSION}/site-packages" \
    -DPython3_LIBRARY="/usr/lib/x86_64-linux-gnu/libpython${PYTHON_VERSION}.so" \
    -DProtobuf_LIBRARY="/usr/lib/x86_64-linux-gnu/libprotobuf.so" \
    -DProtobuf_INCLUDE_DIRS="/usr/include" \
    -DProtobuf_DIR="/usr/lib/x86_64-linux-gnu/cmake/protobuf"

info "Running ./ns3 build ..."
./ns3 build

info "Build complete!"

# ── Step 5: Run pb-wrapper-demo.py ─────────────────────────────
DEMO_PY="$SCRIPT_DIR/pb-wrapper-demo.py"

if [[ ! -f "$DEMO_PY" ]]; then
    error "Demo script not found at $DEMO_PY"
fi

info "Running pb-wrapper-demo.py ..."
cd "$SCRIPT_DIR"
python "$DEMO_PY"

info "All done!"
