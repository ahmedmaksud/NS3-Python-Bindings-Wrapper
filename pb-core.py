#!/usr/bin/env python3
# Copyright (c) 2025 Texas State University
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation;
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Author: Ahmed Maksud <ahmed.maksud@email.ucr.edu>
# PI: Marcelo Menezes De Carvalho <mmcarvalho@txstate.edu>
# Texas State University

"""
pb-core.py - Python Binding AI Controller

This script demonstrates the Python side of NS3-AI communication for
energy-harvesting network optimization:
- Receives network state data from C++ NS3 simulation
- Applies AI optimization algorithms
- Sends optimized control actions back to NS3 simulation

Key Features:
- Complete 23-variable environment handling
- Complete 9-variable action generation
- Robust error handling and logging
- Communication data logging for analysis
- Modular Send2Ns3 wrapper function
"""

import sys
import os

# Add NS3-AI python utilities to path using relative paths only
# From: contrib/ai/examples/pb-wrapper/
# To:   contrib/ai/python_utils/
sys.path.insert(0, os.path.join("..", "..", "python_utils"))

import pb_interface_py as py_binding
from ns3ai_utils import Experiment
import pandas as pd
import traceback
import time
import random


# ANSI color codes for better visibility
class Colors:
    """Color constants for terminal output formatting"""

    RESET = "\033[0m"
    RED = "\033[31m"
    GREEN = "\033[32m"
    YELLOW = "\033[33m"
    BLUE = "\033[34m"
    MAGENTA = "\033[35m"
    CYAN = "\033[36m"


# Global dataframe to store all communication data
communication_data = []


def print_author_info():
    """Print author information"""
    print(f"{Colors.BLUE}========================================{Colors.RESET}")
    print(f"{Colors.BLUE}  Python Binding AI Controller{Colors.RESET}")
    print(f"{Colors.BLUE}  Email: ahmed.maksud@email.ucr.edu{Colors.RESET}")
    print(f"{Colors.BLUE}  Lab: SHINE Lab, Texas State University{Colors.RESET}")
    print(f"{Colors.BLUE}  PI: Marcelo Menezes De Carvalho{Colors.RESET}")
    print(f"{Colors.BLUE}========================================{Colors.RESET}")


def print_action_details(action_struct):
    """
    Print action structure details in yellow
    """
    print(f"{Colors.YELLOW}[Python Wrapper] Send2Ns3 sending action to C++:")

    # Action data with streamlined formatting
    actions = [
        ("set_ApTx", "dBm", ".1f"),
        ("beacon_interval_ms", "ms", ".1f"),
        ("twt_interval_sec", "sec", ".1f"),
        ("enable_smart_mode", "", ""),
        ("adaptation_threshold", "", ".2f"),
        ("coordination_enabled", "", ""),
        ("action_valid", "", ""),
        ("confidence", "", ".2f"),
        ("sta_target", "", ""),
        ("exploration_flag", "", ""),
    ]

    for name, unit, fmt in actions:
        value = action_struct.get(name, -1)
        if fmt:
            formatted_val = f"{value:{fmt}}"
        else:
            formatted_val = str(value)
        unit_str = f" {unit}" if unit else ""
        print(f"  • {name}: {formatted_val}{unit_str}")

    print(Colors.RESET, end="")


def print_environment_details(env):
    """
    Print environment structure details in cyan
    """
    print(f"{Colors.CYAN}[Python Wrapper] Send2Ns3 received environment from C++:")

    # Environment data with streamlined formatting
    env_data = [
        (f"pos_x: {env.pos_x:.1f}, pos_y: {env.pos_y:.1f}",),
        (f"distance: {env.distance:.1f}, sta_id: {env.sta_id}",),
        (f"dl_tp: {env.dl_tp:.1f}, ul_tp: {env.ul_tp:.1f}",),
        (f"get_ApTx: {env.get_ApTx:.1f}, now_sec: {env.now_sec:.1f}",),
        (
            f"harvested_energy: {env.harvested_energy:.3f}, consumed_energy: {env.consumed_energy:.3f}",
        ),
        (
            f"energy_balance: {env.energy_balance:.3f}, output_enabled: {env.output_enabled}",
        ),
        (
            f"capacitor_voltage: {env.capacitor_voltage:.2f}, buffer_occupancy: {env.buffer_occupancy}",
        ),
        (
            f"buffer_overload: {env.buffer_overload}, packet_gen_rate: {env.packet_gen_rate:.1f}",
        ),
        (
            f"smart_mode_enabled: {env.smart_mode_enabled}, ai_confidence: {env.ai_confidence:.2f}",
        ),
        (
            f"adaptation_count: {env.adaptation_count}, learning_rate: {env.learning_rate:.3f}",
        ),
        (f"reward_total: {env.reward_total:.2f}, penalty_count: {env.penalty_count}",),
        (f"coordination_active: {env.coordination_active}",),
    ]

    for line in env_data:
        print(f"  • {line[0]}")

    print(Colors.RESET, end="")


def Send2Ns3(msgInterface, action_struct):
    """
    Python wrapper function: Send action to NS-3 and receive environment data
    NO FALLBACK VALUES - Missing values will be set to -1

    Communication Flow:
    1. Print action data being sent
    2. Receive environment data from C++
    3. Print received environment data
    4. Send action data to C++
    5. Store all communication data for logging

    Args:
        msgInterface: NS-3 AI message interface object
        action_struct: Dictionary containing PBActStruct fields (NO fallbacks)

    Returns:
        env_struct: Dictionary containing all PBEnvStruct fields
    """
    print(
        f"{Colors.GREEN}[Send2Ns3] 🚀 Starting Python→C++ communication cycle{Colors.RESET}"
    )

    # Print action details that will be sent later
    print(f"{Colors.YELLOW}[Send2Ns3] 📤 Preparing action data to send:{Colors.RESET}")
    print_action_details(action_struct)

    # === RECEIVE PHASE: Get environment state from C++ ===
    print(
        f"{Colors.CYAN}[Send2Ns3] 📥 Waiting for environment data from C++...{Colors.RESET}"
    )

    # CRITICAL: Check if simulation has finished BEFORE any blocking operations
    try:
        if msgInterface.PyGetFinished():
            print(
                f"{Colors.RED}[Send2Ns3] ⚠️ Simulation finished signal received - exiting cleanly{Colors.RESET}"
            )
            return None
    except Exception as e:
        print(
            f"{Colors.RED}[Send2Ns3] ⚠️ Error checking simulation status: {e} - assuming finished{Colors.RESET}"
        )
        return None

    # Only proceed with PyRecvBegin if simulation is still running
    try:
        msgInterface.PyRecvBegin()
    except Exception as e:
        print(
            f"{Colors.RED}[Send2Ns3] ⚠️ Error in PyRecvBegin (simulation likely ended): {e}{Colors.RESET}"
        )
        return None

    # Double-check if simulation finished during receive
    try:
        if msgInterface.PyGetFinished():
            print(
                f"{Colors.RED}[Send2Ns3] ⚠️ Simulation finished during receive operation{Colors.RESET}"
            )
            try:
                msgInterface.PyRecvEnd()  # Try to clean up
            except:
                pass
            return None
    except Exception as e:
        print(
            f"{Colors.RED}[Send2Ns3] ⚠️ Error checking simulation status after receive: {e}{Colors.RESET}"
        )
        return None

    # Get environment data from C++
    print(
        f"{Colors.CYAN}[Send2Ns3] 📊 Receiving environment data from C++...{Colors.RESET}"
    )

    try:
        env = msgInterface.GetCpp2PyStruct()
        if env is None:
            print(
                f"{Colors.RED}[Send2Ns3] ⚠️ Received null environment data{Colors.RESET}"
            )
            return None
    except Exception as e:
        print(
            f"{Colors.RED}[Send2Ns3] ⚠️ Error getting environment data: {e}{Colors.RESET}"
        )
        return None

    print(
        f"{Colors.CYAN}[Send2Ns3] ✅ Environment data received successfully{Colors.RESET}"
    )

    # Print environment details
    print_environment_details(env)

    # Create environment dictionary with all 23 variables
    env_struct = {
        # Spatial information (4 variables)
        "pos_x": env.pos_x,
        "pos_y": env.pos_y,
        "distance": env.distance,
        "sta_id": env.sta_id,
        # Network performance metrics (3 variables)
        "dl_tp": env.dl_tp,
        "ul_tp": env.ul_tp,
        "get_ApTx": env.get_ApTx,
        # Temporal information (1 variable)
        "now_sec": env.now_sec,
        # Energy harvesting status (5 variables)
        "harvested_energy": env.harvested_energy,
        "consumed_energy": env.consumed_energy,
        "energy_balance": env.energy_balance,
        "output_enabled": env.output_enabled,
        "capacitor_voltage": env.capacitor_voltage,
        # Buffer management (3 variables)
        "buffer_occupancy": env.buffer_occupancy,
        "buffer_overload": env.buffer_overload,
        "packet_gen_rate": env.packet_gen_rate,
        # Smart node features (4 variables)
        "smart_mode_enabled": env.smart_mode_enabled,
        "ai_confidence": env.ai_confidence,
        "adaptation_count": env.adaptation_count,
        "learning_rate": env.learning_rate,
        # Additional variables (3 variables) for complete 23-variable structure
        "reward_total": env.reward_total,
        "penalty_count": env.penalty_count,
        "coordination_active": env.coordination_active,
    }

    print(
        f"{Colors.CYAN}[Send2Ns3] 🔄 Finalizing environment data reception...{Colors.RESET}"
    )
    msgInterface.PyRecvEnd()
    print(f"{Colors.CYAN}[Send2Ns3] ✅ Environment reception complete{Colors.RESET}")

    # === SEND PHASE: Send action data to C++ ===
    print(
        f"{Colors.YELLOW}[Send2Ns3] 📤 Preparing to send action data to C++...{Colors.RESET}"
    )
    msgInterface.PySendBegin()

    print(
        f"{Colors.YELLOW}[Send2Ns3] 📝 Writing action data to shared memory...{Colors.RESET}"
    )
    action = msgInterface.GetPy2CppStruct()

    # Set all 9 action fields - NO FALLBACKS, use -1 if missing
    action.set_ApTx = action_struct.get("set_ApTx", -1)
    action.beacon_interval_ms = action_struct.get("beacon_interval_ms", -1)
    action.twt_interval_sec = action_struct.get("twt_interval_sec", -1)
    action.enable_smart_mode = action_struct.get("enable_smart_mode", -1)
    action.adaptation_threshold = action_struct.get("adaptation_threshold", -1)
    action.coordination_enabled = action_struct.get("coordination_enabled", -1)
    action.action_valid = action_struct.get("action_valid", -1)
    action.confidence = action_struct.get("confidence", -1)
    action.sta_target = action_struct.get("sta_target", -1)
    action.exploration_flag = action_struct.get("exploration_flag", -1)

    print(f"{Colors.YELLOW}[Send2Ns3] 🚀 Sending action data to C++...{Colors.RESET}")
    msgInterface.PySendEnd()
    print(f"{Colors.YELLOW}[Send2Ns3] ✅ Action data sent successfully{Colors.RESET}")

    # === STORE COMMUNICATION DATA FOR CSV LOGGING ===
    print(
        f"{Colors.MAGENTA}[Send2Ns3] 💾 Storing communication data for logging...{Colors.RESET}"
    )
    communication_record = {
        # Timestamp and metadata
        "timestamp": time.time(),
        # Received environment data (23 variables)
        "env_pos_x": env_struct["pos_x"],
        "env_pos_y": env_struct["pos_y"],
        "env_distance": env_struct["distance"],
        "env_sta_id": env_struct["sta_id"],
        "env_dl_tp": env_struct["dl_tp"],
        "env_ul_tp": env_struct["ul_tp"],
        "env_get_ApTx": env_struct["get_ApTx"],
        "env_now_sec": env_struct["now_sec"],
        "env_harvested_energy": env_struct["harvested_energy"],
        "env_consumed_energy": env_struct["consumed_energy"],
        "env_energy_balance": env_struct["energy_balance"],
        "env_output_enabled": env_struct["output_enabled"],
        "env_capacitor_voltage": env_struct["capacitor_voltage"],
        "env_buffer_occupancy": env_struct["buffer_occupancy"],
        "env_buffer_overload": env_struct["buffer_overload"],
        "env_packet_gen_rate": env_struct["packet_gen_rate"],
        "env_smart_mode_enabled": env_struct["smart_mode_enabled"],
        "env_ai_confidence": env_struct["ai_confidence"],
        "env_adaptation_count": env_struct["adaptation_count"],
        "env_learning_rate": env_struct["learning_rate"],
        "env_reward_total": env_struct["reward_total"],
        "env_penalty_count": env_struct["penalty_count"],
        "env_coordination_active": env_struct["coordination_active"],
        # Sent action data (9 variables)
        "act_set_ApTx": action.set_ApTx,
        "act_beacon_interval_ms": action.beacon_interval_ms,
        "act_twt_interval_sec": action.twt_interval_sec,
        "act_enable_smart_mode": action.enable_smart_mode,
        "act_adaptation_threshold": action.adaptation_threshold,
        "act_coordination_enabled": action.coordination_enabled,
        "act_action_valid": action.action_valid,
        "act_confidence": action.confidence,
        "act_sta_target": action.sta_target,
        "act_exploration_flag": action.exploration_flag,
    }
    communication_data.append(communication_record)

    return env_struct


def initiate_experiment(
    experiment_name="pb_core_simulation",
    ns3_path="../../../..",  # Correct path to ns-3.44 root from pb-wrapper
):
    """
    Initialize Python binding experiment and get message interface

    Args:
        experiment_name: Name of the NS3 C++ program to run
        ns3_path: Path to ns3 executable directory

    Returns:
        msgInterface: Configured message interface for communication
    """
    print(
        f"{Colors.BLUE}[Experiment Init] Starting Python binding experiment: {experiment_name}{Colors.RESET}"
    )

    # Create experiment object to manage NS3-Python communication
    exp = Experiment(
        experiment_name,
        ns3_path,
        py_binding,
        handleFinish=True,
    )

    print(f"{Colors.CYAN}[Experiment Init] Launching NS3 simulation...{Colors.RESET}")
    msgInterface = exp.run(show_output=True)
    print(
        f"{Colors.GREEN}[Experiment Init] NS3 simulation launched successfully{Colors.RESET}"
    )

    return msgInterface, exp


def generate_random_action():
    """
    Generate random action variables for testing
    Returns dictionary with all 9 action variables
    """
    action_struct = {
        # Primary control actions (3 variables)
        "set_ApTx": round(random.uniform(5.0, 25.0), 1),  # 5-25 dBm
        "beacon_interval_ms": round(random.uniform(50.0, 200.0), 1),  # 50-200 ms
        "twt_interval_sec": round(random.uniform(30.0, 120.0), 1),  # 30-120 sec
        # Smart node control (3 variables)
        "enable_smart_mode": random.randint(0, 1),  # 0 or 1
        "adaptation_threshold": round(random.uniform(0.1, 0.9), 2),  # 0.1-0.9
        "coordination_enabled": random.randint(0, 1),  # 0 or 1
        # Action metadata (3 variables)
        "action_valid": random.randint(0, 1),  # 0 or 1
        "confidence": round(random.uniform(0.1, 1.0), 2),  # 0.1-1.0
        "sta_target": random.randint(0, 10),  # 0-10
        "exploration_flag": random.randint(0, 1),  # 0 or 1
    }
    return action_struct


def main():
    """
    Main execution function for Python binding AI Controller
    """
    print_info()
    print(f"{Colors.BLUE}Python Binding AI Controller Starting...{Colors.RESET}")

    # === EXPERIMENT INITIALIZATION WITH WRAPPER ===
    msgInterface, exp = initiate_experiment()

    # Data storage for analysis
    cycle_count = 0

    # === MAIN COMMUNICATION LOOP USING Send2Ns3 WRAPPER ===
    try:
        while True:
            cycle_count += 1
            print(
                f"\n{Colors.BLUE}========== Cycle {cycle_count} - Using Send2Ns3 Wrapper =========={Colors.RESET}"
            )

            # === GENERATE RANDOM ACTION VARIABLES ===
            action_struct = generate_random_action()
            print(
                f"{Colors.MAGENTA}[Random Generation] Generated random action variables{Colors.RESET}"
            )
            print(
                f"  {Colors.MAGENTA}• Generated ApTx: {action_struct['set_ApTx']} dBm{Colors.RESET}"
            )
            print(
                f"  {Colors.MAGENTA}• Generated beacon: {action_struct['beacon_interval_ms']} ms{Colors.RESET}"
            )
            print(
                f"  {Colors.MAGENTA}• Generated smart_mode: {action_struct['enable_smart_mode']}{Colors.RESET}"
            )
            print(
                f"  {Colors.MAGENTA}• Generated confidence: {action_struct['confidence']}{Colors.RESET}"
            )

            # === USE WRAPPER FUNCTION ===
            print(
                f"{Colors.GREEN}[Main Loop] Calling Send2Ns3 wrapper...{Colors.RESET}"
            )
            env_struct = Send2Ns3(msgInterface, action_struct)

            # Check if simulation finished
            if env_struct is None:
                print(f"{Colors.BLUE}[Main Loop] Simulation completed{Colors.RESET}")
                break

            print(
                f"{Colors.GREEN}[Main Loop] Send2Ns3 wrapper completed successfully{Colors.RESET}"
            )

            # === POST-PROCESSING ===
            print(
                f"{Colors.MAGENTA}[Analysis] Processing cycle {cycle_count} results:{Colors.RESET}"
            )
            energy_efficiency = (env_struct["dl_tp"] + env_struct["ul_tp"]) / max(
                0.001, env_struct["consumed_energy"]
            )
            print(
                f"  {Colors.MAGENTA}• Energy Efficiency: {energy_efficiency:.3f} Mbps/J{Colors.RESET}"
            )
            print(
                f"  {Colors.MAGENTA}• Network Performance: {env_struct['dl_tp'] + env_struct['ul_tp']:.1f} Mbps total{Colors.RESET}"
            )
            print(
                f"  {Colors.MAGENTA}• Energy Status: {env_struct['energy_balance']:.3f}J balance{Colors.RESET}"
            )
            print(
                f"  {Colors.MAGENTA}• Smart Features: {env_struct['smart_mode_enabled']} mode, {env_struct['adaptation_count']} adaptations{Colors.RESET}"
            )

            # Brief processing delay
            time.sleep(1)

    # === ERROR HANDLING ===
    except Exception as e:
        """
        Comprehensive error handling for debugging

        """
        exc_type, exc_value, exc_traceback = sys.exc_info()
        print(f"{Colors.RED}Exception occurred: {e}{Colors.RESET}")
        print(f"{Colors.RED}Traceback:{Colors.RESET}")
        traceback.print_tb(exc_traceback)
        exit(1)

    else:
        # Save communication data when simulation completes successfully
        print(
            f"{Colors.BLUE}[Save] Communication data length: {len(communication_data)}{Colors.RESET}"
        )

        # Save comprehensive communication data to pb-wrapper directory
        if communication_data:
            import os

            comm_df = pd.DataFrame(communication_data)
            print(
                f"{Colors.BLUE}[Save] Current working directory: {os.getcwd()}{Colors.RESET}"
            )

            # Use relative path for CSV file in current directory
            csv_file_path = "contrib/ai/examples/pb-wrapper/pb-core-log.csv"

            comm_df.to_csv(csv_file_path, index=False)
            print(
                f"{Colors.GREEN}Communication data saved to {csv_file_path}{Colors.RESET}"
            )
            print(
                f"{Colors.GREEN}Logged {len(communication_data)} communication cycles{Colors.RESET}"
            )
            print(
                f"{Colors.CYAN}Communication log contains all 23 environment + 9 action variables{Colors.RESET}"
            )
        else:
            print(f"{Colors.RED}[Warning] No communication data to save!{Colors.RESET}")

        # === DEMONSTRATION: Using Send2Ns3 Wrapper ===
        print(f"\n{Colors.BLUE}=== DEMONSTRATING Send2Ns3 WRAPPER ==={Colors.RESET}")
        print(
            f"{Colors.CYAN}Usage: env_struct = Send2Ns3(msgInterface, action_struct){Colors.RESET}"
        )
        print(
            f"{Colors.CYAN}This wrapper handles all variables from the .h file automatically{Colors.RESET}"
        )
        print(
            f"{Colors.CYAN}NO FALLBACK VALUES - Missing values set to -1{Colors.RESET}"
        )
        print(f"{Colors.CYAN}Implementation{Colors.RESET}")

        print(
            f"{Colors.GREEN}The Send2Ns3 wrapper automatically handles:{Colors.RESET}"
        )
        print(
            "  • All 23 environment variables (pos_x, pos_y, distance, sta_id, dl_tp, ul_tp, etc.)"
        )
        print(
            "  • All 9 action variables (set_ApTx, beacon_interval_ms, twt_interval_sec, etc.)"
        )
        print("  • Proper synchronization (PyRecvBegin/End, PySendBegin/End)")
        print("  • Error handling (simulation finish detection)")
        print("  • Communication logging (all variables saved to CSV)")
        print("  • No fallback values (missing values = -1)")

    # === CLEANUP ===
    finally:
        """
        Cleanup code that always executes

        """
        print(f"{Colors.BLUE}Finally exiting...{Colors.RESET}")
        del exp  # Clean up experiment object and shared memory


if __name__ == "__main__":
    main()
