# NS3-AI Python Bindings for Energy Harvesting Networks

[![License: GPL v2](https://img.shields.io/badge/License-GPL_v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)
[![NS-3 Version](https://img.shields.io/badge/NS--3-3.44-green.svg)](https://www.nsnam.org/releases/ns-3-44/)
[![Python](https://img.shields.io/badge/Python-3.11+-blue.svg)](https://www.python.org/)
[![NS3-AI](https://img.shields.io/badge/Based%20on-NS3--AI-orange.svg)](https://github.com/hust-diangroup/ns3-ai)

A comprehensive Python binding interface for NS-3 AI communication in energy harvesting wireless networks, designed for MobiCom research projects.

**Author**: Ahmed Maksud (amaks002@ucr.edu)  
**Lab**: SHINE Lab, Texas State University  
**PI**: Marcelo Menezes De Carvalho

---

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Data Structures](#data-structures)
- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Usage Examples](#usage-examples)
- [Architecture](#architecture)
- [GitHub Repository Setup](#github-repository-setup)
- [Project Structure](#project-structure)
- [Contributing](#contributing)
- [License](#license)

---

## Overview

This project is built upon the [ns3-ai](https://github.com/hust-diangroup/ns3-ai) framework, extending it with specialized data structures and workflows optimized for energy harvesting network research. It provides seamless bidirectional communication between NS-3 C++ simulations and Python AI controllers.

### Key Research Areas
- RF Energy Harvesting Optimization
- AI-Driven Network Control
- PowerCast Hardware Integration
- Target Wake Time (TWT) Coordination
- Energy-Efficient Wireless Communication

---

## Features

- **Bidirectional Communication**: Real-time data exchange between NS-3 C++ and Python via ns3-ai shared memory
- **Energy Harvesting Focus**: 23 environment variables tailored for RF energy harvesting networks
- **Complete Control**: 9 action variables for comprehensive network optimization
- **Robust Error Handling**: Graceful simulation termination and error detection
- **Communication Logging**: CSV logging of all communication events
- **Modular Design**: Clean wrapper functions for easy integration
- **Ready for ML/RL**: Direct integration with TensorFlow, PyTorch, or custom algorithms

---

## Data Structures

### Environment Variables (23 total)

| Category | Variables | Description |
|----------|-----------|-------------|
| **Spatial (4)** | `pos_x`, `pos_y`, `distance`, `sta_id` | Node position and identification |
| **Network Metrics (3)** | `dl_tp`, `ul_tp`, `get_ApTx` | Throughput and transmission power |
| **Temporal (1)** | `now_sec` | Current simulation time |
| **Energy Harvesting (5)** | `harvested_energy`, `consumed_energy`, `energy_balance`, `output_status`, `cap_voltage` | Energy system status |
| **Buffer Management (3)** | `buffer_occupancy`, `buffer_overload`, `gen_rate` | Queue and traffic state |
| **Smart Features (4)** | `smart_mode`, `ai_confidence`, `adaptation_count`, `learning_rate` | AI controller state |
| **Additional (3)** | `total_reward`, `penalty_count`, `coordination_flag` | Performance metrics |

### Action Variables (9 total)

| Variable | Type | Description |
|----------|------|-------------|
| `set_ApTx` | float | AP transmission power (dBm) |
| `beacon_interval_ms` | float | Beacon interval (milliseconds) |
| `twt_interval_sec` | float | TWT sleep interval (seconds) |
| `enable_smart_mode` | int | Enable/disable smart mode (0/1) |
| `adaptation_threshold` | float | Adaptation threshold (0.0-1.0) |
| `coordination_enabled` | int | Enable coordination (0/1) |
| `action_valid` | int | Action validity flag (0/1) |
| `confidence` | float | AI confidence level (0.0-1.0) |
| `sta_target` | int | Target station ID |

---

## Prerequisites

1. **NS-3.44** (or compatible version)
2. **ns3-ai module** installed in `contrib/ai`
3. **Python 3.11+** with pandas and numpy
4. **pybind11** (bundled with ns3-ai)

---

## Quick Start

### 1. Installation
See [INSTALL.md](INSTALL.md) for detailed instructions.

### 2. Run Demo
```bash
# Build NS-3
cd /path/to/ns-3.44
./ns3 build pb_wrapper_demo

# Run C++ simulation
./ns3 run pb_wrapper_demo

# In another terminal: Run Python AI controller
cd build/contrib/ai/examples/MobiCom/
python3 pb-wrapper-demo.py
```

### 3. Basic Python Controller
```python
from pb_core import Send2Ns3, initiate_experiment

# Initialize
msgInterface, exp = initiate_experiment("my_simulation")

while True:
    # Create action
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
        "exploration_flag": 0
    }
    
    # Send action, receive environment
    env = Send2Ns3(msgInterface, action)
    if env is None:
        break
    
    # Your AI logic here
    print(f"Energy: {env['energy_balance']:.2f} J, Throughput: {env['dl_tp']:.1f} Mbps")
```

### 4. Basic C++ Integration
```cpp
#include "pb-wrapper.h"

Ptr<PBWrapper> wrapper = CreateObject<PBWrapper>();
wrapper->Initialize();

// In simulation loop
PBEnvStruct env;
env.pos_x = 10.0;
env.pos_y = 5.0;
env.distance = 10.0;
env.dl_tp = 50.0;
env.harvested_energy = 10.0;
// ... set all 23 variables

PBActStruct action = wrapper->SendToPython(env);

if (action.action_valid == 1) {
    // Apply actions to your network
    SetApTxPower(action.set_ApTx);
}
```

---

## Usage Examples

### Example 1: Simple Energy-Aware Controller
```python
def simple_energy_controller():
    msgInterface, exp = initiate_experiment("energy_control")
    
    while True:
        action = {"set_ApTx": 20.0, "action_valid": 1, ...}  # Default action
        env = Send2Ns3(msgInterface, action)
        if env is None:
            break
        
        # Adjust power based on energy
        if env["energy_balance"] > 5.0:
            action["set_ApTx"] = 25.0  # Increase power
        elif env["energy_balance"] < 0:
            action["set_ApTx"] = 15.0  # Decrease power
```

### Example 2: Q-Learning Controller
```python
import numpy as np

class QLearningController:
    def __init__(self):
        self.q_table = {}
        self.alpha = 0.1
        self.gamma = 0.95
        self.epsilon = 0.1
        self.power_levels = [10, 15, 20, 25, 30]
    
    def discretize_state(self, env):
        energy_level = 0 if env["energy_balance"] < 0 else 1 if env["energy_balance"] < 5 else 2
        buffer_level = 0 if env["buffer_occupancy"] < 0.3 else 1 if env["buffer_occupancy"] < 0.7 else 2
        return (energy_level, buffer_level)
    
    def choose_action(self, state):
        if np.random.random() < self.epsilon:
            return np.random.randint(0, len(self.power_levels))
        return np.argmax(self.q_table.get(state, np.zeros(len(self.power_levels))))
    
    def update(self, state, action, reward, next_state):
        current_q = self.q_table.get(state, np.zeros(len(self.power_levels)))[action]
        max_next_q = np.max(self.q_table.get(next_state, np.zeros(len(self.power_levels))))
        new_q = current_q + self.alpha * (reward + self.gamma * max_next_q - current_q)
        if state not in self.q_table:
            self.q_table[state] = np.zeros(len(self.power_levels))
        self.q_table[state][action] = new_q

# Use in simulation
controller = QLearningController()
msgInterface, exp = initiate_experiment("qlearning")
prev_state = None

while True:
    action_dict = {"set_ApTx": 20.0, "action_valid": 1, ...}
    env = Send2Ns3(msgInterface, action_dict)
    if env is None:
        break
    
    state = controller.discretize_state(env)
    action_idx = controller.choose_action(state)
    action_dict["set_ApTx"] = controller.power_levels[action_idx]
    
    if prev_state is not None:
        reward = env["dl_tp"] - abs(env["energy_balance"]) if env["energy_balance"] < 0 else 0
        controller.update(prev_state, prev_action_idx, reward, state)
    
    prev_state = state
    prev_action_idx = action_idx
```

### Example 3: Multi-Objective Optimization
```python
class MultiObjectiveController:
    def __init__(self, weights={'energy': 0.4, 'throughput': 0.4, 'latency': 0.2}):
        self.weights = weights
    
    def calculate_score(self, env):
        energy_score = env["energy_balance"] / max(1, env["consumed_energy"])
        throughput_score = (env["dl_tp"] + env["ul_tp"]) / 100.0
        latency_score = 1.0 - env["buffer_occupancy"]
        return (self.weights['energy'] * energy_score +
                self.weights['throughput'] * throughput_score +
                self.weights['latency'] * latency_score)
    
    def optimize(self, env):
        action = {"set_ApTx": 20.0, "twt_interval_sec": 60.0, "action_valid": 1, ...}
        
        if env["energy_balance"] < 0:
            action["set_ApTx"] = max(10.0, action["set_ApTx"] - 5.0)
            action["twt_interval_sec"] = min(120.0, action["twt_interval_sec"] + 10.0)
        
        if env["buffer_occupancy"] > 0.8:
            action["set_ApTx"] = min(30.0, action["set_ApTx"] + 5.0)
        
        return action
```

---

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
│            ↓                             │
│    ┌──────────────────────────────┐     │
│    │  Your Network Simulation     │     │
│    │  (WiFi, Energy Harvesting)   │     │
│    └──────────────────────────────┘     │
└─────────────────────────────────────────┘
```

---

## GitHub Repository Setup

### Creating the Repository

1. **Create on GitHub**:
   - Go to https://github.com → New repository
   - Name: `ns3-python-bindings`
   - Description: `NS3-AI Python Bindings for Energy Harvesting Networks`
   - Public/Private as needed
   - Don't initialize (we have files already)

2. **Push from local**:
   ```bash
   cd /home/mak/NS3-project/ns-allinone-3.44/ns-3.44/contrib/ai/examples/MobiCom/python_bindings
   git init
   git add .
   git commit -m "Initial commit: NS3-AI Python Bindings"
   git remote add origin https://github.com/YOUR_USERNAME/ns3-python-bindings.git
   git branch -M main
   git push -u origin main
   ```

### User Installation

Users clone and install with:
```bash
git clone https://github.com/YOUR_USERNAME/ns3-python-bindings.git
cp -r ns3-python-bindings ns-3.44/contrib/ai/examples/MobiCom/python_bindings
cd ns-3.44
./ns3 configure --enable-examples
./ns3 build
```

See [INSTALL.md](INSTALL.md) for detailed step-by-step instructions.

### Directory Structure in NS-3

After installation:
```
ns-3.44/contrib/ai/examples/
├── CMakeLists.txt              # Add: add_subdirectory(MobiCom)
└── MobiCom/
    ├── CMakeLists.txt          # Add: add_subdirectory(python_bindings)
    └── python_bindings/        # ← This repository
        ├── README.md
        ├── INSTALL.md
        ├── CMakeLists.txt
        └── source files...
```

---

## Project Structure

```
python_bindings/
├── README.md              # This file - complete documentation
├── INSTALL.md             # Detailed installation guide
├── LICENSE                # GPL v2 License
├── .gitignore             # Git ignore rules
├── CMakeLists.txt         # Build configuration
├── pb-core.h              # Core data structures
├── pb-core.cc             # Core implementation
├── pb-core.py             # Python AI controller interface
├── pb-wrapper.h           # Clean wrapper interface
├── pb-wrapper.cc          # Wrapper implementation
├── pb-wrapper-demo.cc     # C++ demonstration
├── pb-wrapper-demo.py     # Python demonstration
└── pb-interface.cc        # Python bindings (pybind11)
```

---

## Contributing

Contributions are welcome! To contribute:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

---

## License

This project is licensed under the GNU General Public License v2.0 - see the [LICENSE](LICENSE) file for details.

---

## Contact

**Ahmed Maksud**  
Email: amaks002@ucr.edu  
Lab: SHINE Lab, Texas State University  
PI: Marcelo Menezes De Carvalho

For issues or questions, please open an issue on GitHub or email directly.

---

## Acknowledgments

- [ns3-ai](https://github.com/hust-diangroup/ns3-ai) - Core framework for NS-3 AI integration
- NS-3 Development Team
- SHINE Lab research team
- Texas State University
