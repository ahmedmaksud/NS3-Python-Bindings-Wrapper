# NS3-AI Python Bindings for Energy Harvesting Networks

**Author:** Ahmed Maksud (ahmed.maksud@email.ucr.edu)  
**PI:** Marcelo Menezes De Carvalho (mmcarvalho@txstate.edu)  
**Institution:** Texas State University

A Python binding interface for NS3-AI communication in energy harvesting wireless networks, providing bidirectional data exchange between NS-3 C++ simulations and Python AI controllers.

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)
[![NS3 Version](https://img.shields.io/badge/NS3-3.44-blue.svg)](https://www.nsnam.org/)
[![Python](https://img.shields.io/badge/Python-3.11-green.svg)](https://www.python.org/)

_For NS3-AI integration details, check [NS3-AI GitHub](https://github.com/hust-diangroup/ns3-ai)_

> **Important**: This project requires the [NS3-NS3AI installation repository](https://github.com/ahmedmaksud/NS3-NS3AI--installation-and-tests.git) to be completed first. NS3 and NS3-AI must already be installed and working.

> It also assumes the same virtual environment ('EHRL') as the [NS3-NS3AI installation repository](https://github.com/ahmedmaksud/NS3-NS3AI--installation-and-tests.git).

## Installation and Setup

**Prerequisites**: Complete the [NS3-NS3AI installation repository](https://github.com/ahmedmaksud/NS3-NS3AI--installation-and-tests.git) first.

### Quick Installation

```bash
# Navigate to the NS3 examples directory
cd /path/to/NS3-project/ns-allinone-3.44/ns-3.44/contrib/ai/examples/

# Clone this repository with the correct folder name
git clone https://github.com/ahmedmaksud/NS3-Python-Bindings-Wrapper.git pb-wrapper

# Navigate into the directory
cd pb-wrapper

# Make setup script executable and run
chmod +x setup-and-run.sh
./setup-and-run.sh
```

The `setup-and-run.sh` script will automatically:

1. Rename the directory to `pb-wrapper` (if needed)
2. Add `add_subdirectory(pb-wrapper)` to the parent CMakeLists.txt (if needed)
3. Activate the EHRL virtual environment
4. Clean, configure, and build NS-3
5. Run the demo (`pb-wrapper-demo.py`)

**Directory Structure Required**:

```
NS3-project/
├── EHRL/                          # Python virtual environment
├── ns-allinone-3.44/
│   └── ns-3.44/
│       └── contrib/ai/
│           └── examples/
│               ├── CMakeLists.txt # Must include: add_subdirectory(pb-wrapper)
│               └── pb-wrapper/    # ← This repository
└── NS3-NS3AI--installation-and-tests/  # Previous repository (required)
```

## Overview

This project is built upon the [ns3-ai](https://github.com/hust-diangroup/ns3-ai) framework, extending it with specialized data structures and workflows optimized for energy harvesting network research.

### Features

- **Bidirectional Communication**: Real-time data exchange between NS-3 C++ and Python via shared memory
- **Energy Harvesting Focus**: 23 environment variables tailored for RF energy harvesting networks
- **Complete Control**: 9 action variables for comprehensive network optimization
- **Robust Error Handling**: Graceful simulation termination and error detection
- **Communication Logging**: CSV logging of all communication events
- **Modular Design**: Clean wrapper functions for easy integration
- **Ready for ML/RL**: Direct integration with TensorFlow, PyTorch, or custom algorithms

## Data Structures

### Environment Variables (23 total)

| Category                  | Variables                                                                               | Description                       |
| ------------------------- | --------------------------------------------------------------------------------------- | --------------------------------- |
| **Spatial (4)**           | `pos_x`, `pos_y`, `distance`, `sta_id`                                                  | Node position and identification  |
| **Network Metrics (3)**   | `dl_tp`, `ul_tp`, `get_ApTx`                                                            | Throughput and transmission power |
| **Temporal (1)**          | `now_sec`                                                                               | Current simulation time           |
| **Energy Harvesting (5)** | `harvested_energy`, `consumed_energy`, `energy_balance`, `output_status`, `cap_voltage` | Energy system status              |
| **Buffer Management (3)** | `buffer_occupancy`, `buffer_overload`, `gen_rate`                                       | Queue and traffic state           |
| **Smart Features (4)**    | `smart_mode`, `ai_confidence`, `adaptation_count`, `learning_rate`                      | AI controller state               |
| **Additional (3)**        | `total_reward`, `penalty_count`, `coordination_flag`                                    | Performance metrics               |

### Action Variables (9 total)

| Variable               | Type  | Description                     |
| ---------------------- | ----- | ------------------------------- |
| `set_ApTx`             | float | AP transmission power (dBm)     |
| `beacon_interval_ms`   | float | Beacon interval (milliseconds)  |
| `twt_interval_sec`     | float | TWT sleep interval (seconds)    |
| `enable_smart_mode`    | int   | Enable/disable smart mode (0/1) |
| `adaptation_threshold` | float | Adaptation threshold (0.0-1.0)  |
| `coordination_enabled` | int   | Enable coordination (0/1)       |
| `action_valid`         | int   | Action validity flag (0/1)      |
| `confidence`           | float | AI confidence level (0.0-1.0)   |
| `sta_target`           | int   | Target station ID               |

## Architecture

```
┌─────────────────────────────────────────┐
│    Python AI Controller (pb-core.py)    │
│    ┌──────────────────────────────┐     │
│    │  Your ML/RL Algorithm        │     │
│    │  (TensorFlow, PyTorch, etc.) │     │
│    └──────────────────────────────┘     │
│            ↓ Send2Ns3 ↑                 │
└────────────────┬────────────────────────┘
                 │ ns3-ai shared memory
┌────────────────┴────────────────────────┐
│    NS-3 C++ Simulation                  │
│    ┌──────────────────────────────┐     │
│    │  PBWrapper::SendToPython()   │     │
│    │  23 env vars → 9 action vars │     │
│    └──────────────────────────────┘     │
│            ↓                            │
│    ┌──────────────────────────────┐     │
│    │  Your Network Simulation     │     │
│    │  (WiFi, Energy Harvesting)   │     │
│    └──────────────────────────────┘     │
└─────────────────────────────────────────┘
```

## Quick Start

### Python Controller

```python
from pb_core import Send2Ns3, initiate_experiment

msgInterface, exp = initiate_experiment("my_simulation")

while True:
    action = {
        "set_ApTx": 20.0,
        "beacon_interval_ms": 100.0,
        "twt_interval_sec": 60.0,
        "enable_smart_mode": 1,
        "adaptation_threshold": 0.5,
        "coordination_enabled": 1,
        "action_valid": 1,
        "confidence": 0.9,
        "sta_target": 0,
    }

    env = Send2Ns3(msgInterface, action)
    if env is None:
        break

    print(f"Energy: {env['energy_balance']:.2f} J, Throughput: {env['dl_tp']:.1f} Mbps")
```

### C++ Integration

```cpp
#include "pb-wrapper.h"

Ptr<PBWrapper> wrapper = CreateObject<PBWrapper>();
wrapper->Initialize();

PBEnvStruct env;
env.pos_x = 10.0;
env.dl_tp = 50.0;
env.harvested_energy = 10.0;
// ... set all 23 variables

PBActStruct action = wrapper->SendToPython(env);

if (action.action_valid == 1) {
    SetApTxPower(action.set_ApTx);
}
```

## File Structure

```
pb-wrapper/
├── README.md              # This file
├── LICENSE                # GPL v2 License
├── CMakeLists.txt         # Build configuration
├── setup-and-run.sh       # Automated setup, build, and run script
├── pb-core.h              # Core data structures (PBEnvStruct, PBActStruct)
├── pb-core.cc             # Core implementation (Send2Python, etc.)
├── pb-core.py             # Python AI controller interface
├── pb-wrapper.h           # Clean wrapper class interface
├── pb-wrapper.cc          # Wrapper implementation
├── pb-wrapper-demo.cc     # C++ demonstration
├── pb-wrapper-demo.py     # Python demonstration
└── pb-interface.cc        # Python bindings (pybind11)
```

## Manual Build (without setup script)

If you prefer to build manually instead of using `setup-and-run.sh`:

```bash
# 1. Add subdirectory to parent CMake (if not already done)
cd contrib/ai/examples/
grep -q "pb-wrapper" CMakeLists.txt || echo "add_subdirectory(pb-wrapper)" >> CMakeLists.txt

# 2. Configure and build from NS-3 root
cd ../../..
./ns3 configure --enable-examples
./ns3 build

# 3. Run (two terminals)
# Terminal 1:
./ns3 run pb_wrapper_demo
# Terminal 2:
cd contrib/ai/examples/pb-wrapper/
python3 pb-wrapper-demo.py
```

## Troubleshooting

### Common Issues

1. **`powercast_hardware not found`** — Edit `CMakeLists.txt` and comment out the `../powercast_hardware/` references in both `pb_core_sources` and `pb_wrapper_sources`.

2. **`ImportError: No module named 'pb_interface_py'`** — Run the Python script from the correct directory (`contrib/ai/examples/pb-wrapper/` or the build output directory).

3. **`Shared memory connection failed`** — Ensure both C++ and Python processes are running simultaneously. Start C++ first, then Python.

4. **`Python.h: No such file or directory`** — Install Python dev headers: `sudo apt-get install python3-dev`

5. **`add_subdirectory not found`** — Verify `contrib/ai/examples/CMakeLists.txt` contains `add_subdirectory(pb-wrapper)`.
