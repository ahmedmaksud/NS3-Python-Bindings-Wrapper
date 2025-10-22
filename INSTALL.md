# Installation Guide

Complete step-by-step installation instructions for NS3-AI Python Bindings.

## Table of Contents
- [Prerequisites](#prerequisites)
- [Current Directory Structure](#current-directory-structure)
- [Installation Steps](#installation-steps)
- [CMake Configuration](#cmake-configuration)
- [Building the Project](#building-the-project)
- [Testing Installation](#testing-installation)
- [Troubleshooting](#troubleshooting)

---

## Prerequisites

### Required Software
1. **NS-3.44** - Download from https://www.nsnam.org/releases/ns-3-44/
2. **ns3-ai module** - Must be installed in `contrib/ai`:
   ```bash
   cd contrib/
   git clone https://github.com/hust-diangroup/ns3-ai.git ai
   cd ..
   ```
3. **Python 3.11+**
   ```bash
   python3 --version  # Should be 3.11 or higher
   pip3 install pandas numpy
   ```
4. **Build Tools** - CMake 3.10+, g++/clang with C++17 support

---

## Current Directory Structure

This repository should be placed in the following location within your NS-3 installation:

```
ns-3.44/                                    # NS-3 root directory
└── contrib/
    └── ai/                                 # ns3-ai module
        └── examples/
            └── MobiCom/
                └── python_bindings/        # ← This repository goes here
                    ├── README.md
                    ├── INSTALL.md          # This file
                    ├── LICENSE
                    ├── .gitignore
                    ├── CMakeLists.txt
                    ├── pb-core.h
                    ├── pb-core.cc
                    ├── pb-core.py
                    ├── pb-wrapper.h
                    ├── pb-wrapper.cc
                    ├── pb-wrapper-demo.cc
                    ├── pb-wrapper-demo.py
                    └── pb-interface.cc
```

---

## Installation Steps

### Step 1: Clone the Repository

From any location, clone this repository:

```bash
git clone https://github.com/YOUR_USERNAME/ns3-python-bindings.git
```

### Step 2: Navigate to Your NS-3 Installation

```bash
cd ns-3.44/  # Your NS-3 root directory
```

### Step 3: Create MobiCom Directory (if it doesn't exist)

```bash
mkdir -p contrib/ai/examples/MobiCom
```

### Step 4: Copy Repository to NS-3

From the NS-3 root directory:

```bash
cp -r /path/to/ns3-python-bindings/* contrib/ai/examples/MobiCom/python_bindings/
```

Or if you cloned in a temporary location:

```bash
# If you cloned to /tmp/ns3-python-bindings
cp -r /tmp/ns3-python-bindings contrib/ai/examples/MobiCom/python_bindings
```

### Step 5: Update Parent CMakeLists.txt Files

#### File 1: `contrib/ai/examples/CMakeLists.txt`

Check if `MobiCom` subdirectory is added:

```bash
# From NS-3 root directory
grep "MobiCom" contrib/ai/examples/CMakeLists.txt
```

If not found, add it:

```bash
echo "add_subdirectory(MobiCom)" >> contrib/ai/examples/CMakeLists.txt
```

Or edit manually:

```cmake
# contrib/ai/examples/CMakeLists.txt
add_subdirectory(MobiCom)
```

#### File 2: `contrib/ai/examples/MobiCom/CMakeLists.txt`

Create if it doesn't exist:

```bash
cat > contrib/ai/examples/MobiCom/CMakeLists.txt << 'EOF'
# MobiCom Examples CMakeLists.txt
add_subdirectory(python_bindings)
EOF
```

Or if you have other modules in MobiCom:

```cmake
# contrib/ai/examples/MobiCom/CMakeLists.txt
add_subdirectory(powercast_hardware)    # if you have it
add_subdirectory(twt)                   # if you have it
add_subdirectory(action_schedule)       # if you have it
add_subdirectory(python_bindings)       # This module
```

---

## Building the Project

### Step 1: Configure NS-3

From NS-3 root directory:

```bash
./ns3 configure --enable-examples
```

### Step 2: Build

```bash
./ns3 build
```

Or build with multiple cores for faster compilation:

```bash
./ns3 build -j$(nproc)
```

### Step 3: Verify Build Output

Check that executables were created:

```bash
ls -la build/contrib/ai/examples/MobiCom/
```

You should see:
- `pb_core_simulation` (executable)
- `pb_wrapper_demo` (executable)
- `pb-core.py` (Python script)
- `pb-wrapper-demo.py` (Python script)
- `pb_interface_py.so` (Python binding library)

---

## CMake Configuration

### Understanding the Build System

The `CMakeLists.txt` in `python_bindings/` handles:
1. Building C++ libraries (`pb-core.cc`, `pb-wrapper.cc`)
2. Creating executables (`pb_core_simulation`, `pb_wrapper_demo`)
3. Compiling Python bindings (`pb_interface_py.so` via pybind11)
4. Deploying Python scripts to build directory

### Required NS-3 Libraries

The module depends on these libraries (automatically linked):
- `libai` - ns3-ai core functionality
- `libcore`, `libnetwork`, `libwifi`
- `libmobility`, `libinternet`
- `libapplications`, `libflow-monitor`
- `libpoint-to-point`, `libenergy`

### Optional Dependencies

If you have the complete MobiCom project with PowerCast hardware module, the CMakeLists.txt includes:

```cmake
../powercast_hardware/ph-harvester-hardware.cc
../powercast_hardware/ph-deployment-helper.cc
```

**If you DON'T have powercast_hardware**, edit `python_bindings/CMakeLists.txt`:

From NS-3 root:
```bash
nano contrib/ai/examples/MobiCom/python_bindings/CMakeLists.txt
```

Comment out these lines in both `pb_core_sources` and `pb_wrapper_sources`:
```cmake
set(pb_core_sources
    pb-core.cc
    # ../powercast_hardware/ph-harvester-hardware.cc    # COMMENTED
    # ../powercast_hardware/ph-deployment-helper.cc     # COMMENTED
)

set(pb_wrapper_sources
    pb-wrapper-demo.cc
    pb-wrapper.cc
    # ../powercast_hardware/ph-harvester-hardware.cc    # COMMENTED
    # ../powercast_hardware/ph-deployment-helper.cc     # COMMENTED
)
```

---

## Testing Installation

### Test 1: Run C++ Demo

From NS-3 root directory:

```bash
./ns3 run pb_wrapper_demo
```

Expected output:
```
=== PBWrapper Demo ===
Initializing wrapper...
Demo running...
```

### Test 2: Run Python Demo

First terminal (from NS-3 root):
```bash
./ns3 run pb_wrapper_demo
```

Second terminal (navigate to build directory):
```bash
cd build/contrib/ai/examples/MobiCom/
python3 pb-wrapper-demo.py
```

Expected output:
```
=== Python Bindings Demo ===
Connecting to NS-3...
Communication successful!
```

### Test 3: Verify Files

From NS-3 root:

```bash
# Check executables
ls -l build/contrib/ai/examples/MobiCom/pb_*

# Check Python files
ls -l build/contrib/ai/examples/MobiCom/*.py

# Check binding library
ls -l build/contrib/ai/examples/MobiCom/pb_interface_py.so
```

---

## Troubleshooting

### Build Errors

**Error: `fatal error: ns3-ai not found`**

Solution:
```bash
# Check if ns3-ai is installed
ls contrib/ai/model/

# If not found, install it
cd contrib/
git clone https://github.com/hust-diangroup/ns3-ai.git ai
cd ..
./ns3 configure
./ns3 build
```

**Error: `Python.h: No such file or directory`**

Solution:
```bash
# Ubuntu/Debian
sudo apt-get install python3-dev

# RHEL/CentOS/Fedora
sudo yum install python3-devel

# Arch Linux
sudo pacman -S python
```

**Error: `powercast_hardware not found`**

Solution: Edit `contrib/ai/examples/MobiCom/python_bindings/CMakeLists.txt` and comment out powercast_hardware references (see [Optional Dependencies](#optional-dependencies))

**Error: `CMake Error: add_subdirectory not found`**

Solution: Verify parent CMakeLists.txt files:
```bash
# Check if MobiCom is added
grep "MobiCom" contrib/ai/examples/CMakeLists.txt

# Check if python_bindings is added
grep "python_bindings" contrib/ai/examples/MobiCom/CMakeLists.txt
```

### Runtime Errors

**Error: `ImportError: No module named 'pb_core'`**

Solution: Run Python scripts from the build directory:
```bash
cd build/contrib/ai/examples/MobiCom/
python3 pb-wrapper-demo.py
```

Or set PYTHONPATH:
```bash
export PYTHONPATH="build/contrib/ai/examples/MobiCom:$PYTHONPATH"
python3 -c "from pb_core import Send2Ns3"
```

**Error: `pb_interface_py.so not found`**

Solution:
```bash
# Check if it was built
ls build/contrib/ai/examples/MobiCom/pb_interface_py.so

# If missing, rebuild
./ns3 clean
./ns3 configure --enable-examples
./ns3 build
```

**Error: `Shared memory connection failed`**

Solution: Ensure both C++ and Python processes are running:
1. Start C++ simulation first: `./ns3 run pb_wrapper_demo`
2. Then start Python controller: `cd build/contrib/ai/examples/MobiCom/ && python3 pb-wrapper-demo.py`

### Configuration Issues

**Build is slow**

Solution: Use parallel compilation:
```bash
./ns3 build -j$(nproc)  # Uses all CPU cores
```

**Changes not reflected after rebuild**

Solution: Clean and rebuild:
```bash
./ns3 clean
./ns3 configure --enable-examples
./ns3 build
```

---

## Advanced Configuration

### Running from Different Directories

If you want to run Python scripts from anywhere:

```bash
# From NS-3 root
export PYTHONPATH="$(pwd)/build/contrib/ai/examples/MobiCom:$PYTHONPATH"
export LD_LIBRARY_PATH="$(pwd)/build/lib:$LD_LIBRARY_PATH"

# Now you can run from any directory
python3 -c "from pb_core import Send2Ns3; print('Success!')"
```

### Development Workflow

When making changes to the code:

```bash
# 1. Edit source files in contrib/ai/examples/MobiCom/python_bindings/

# 2. Rebuild only changed components
./ns3 build pb_wrapper_demo

# 3. Test immediately
./ns3 run pb_wrapper_demo
```

### Custom NS-3 Build Directory

If your NS-3 uses a custom build directory:

```bash
# Configure with custom build directory
./ns3 configure --enable-examples --out=mybuild

# Files will be in
ls mybuild/contrib/ai/examples/MobiCom/
```

---

## Summary: Quick Installation Checklist

- [ ] NS-3.44 installed
- [ ] ns3-ai module in `contrib/ai/`
- [ ] Python 3.11+ with pandas and numpy
- [ ] Repository cloned
- [ ] Files copied to `contrib/ai/examples/MobiCom/python_bindings/`
- [ ] `contrib/ai/examples/CMakeLists.txt` includes MobiCom
- [ ] `contrib/ai/examples/MobiCom/CMakeLists.txt` includes python_bindings
- [ ] `./ns3 configure --enable-examples` executed
- [ ] `./ns3 build` completed successfully
- [ ] Executables exist in `build/contrib/ai/examples/MobiCom/`
- [ ] Demo runs successfully

---

## Getting Help

If you encounter issues:

1. **Check this guide** - Review [Troubleshooting](#troubleshooting) section
2. **Verify installation** - Use [Testing Installation](#testing-installation) steps
3. **Check NS-3 documentation** - https://www.nsnam.org/documentation/
4. **Email support** - amaks002@ucr.edu
5. **Open GitHub issue** - https://github.com/YOUR_USERNAME/ns3-python-bindings/issues

---

**Author**: Ahmed Maksud (amaks002@ucr.edu)  
**Lab**: SHINE Lab, Texas State University  
**PI**: Marcelo Menezes De Carvalho
