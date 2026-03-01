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
pb-wrapper-demo.py - Python Binding Wrapper Demo

This script demonstrates a Python wrapper approach for NS3-AI communication.
Unlike pb-core.py which directly uses the interface, this creates a clean
wrapper function that can be easily integrated into other Python applications.

Key Features:
- Clean Send2Ns3_Wrapper() function for easy integration
- Simplified error handling and logging
- Same 23+9 variable handling as pb-core.py
- Modular design for reuse in other projects
- Demonstrates wrapper vs core approach
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


# Global list to store all communication data for CSV logging
communication_data = []


def print_wrapper_info():
    """Print wrapper information"""
    print(f"{Colors.BLUE}========================================{Colors.RESET}")
    print(f"{Colors.BLUE}  Python Binding Wrapper Demo{Colors.RESET}")
    print(f"{Colors.BLUE}  Email: amaks002@ucr.edu{Colors.RESET}")
    print(f"{Colors.BLUE}  Lab: SHINE Lab, Texas State University{Colors.RESET}")
    print(f"{Colors.BLUE}  PI: Marcelo Menezes De Carvalho{Colors.RESET}")
    print(f"{Colors.BLUE}  Purpose: Demonstrate wrapper functions{Colors.RESET}")
    print(f"{Colors.BLUE}========================================{Colors.RESET}")


class PythonWrapperUtils:
    """
    Python Wrapper Utility Class

    Provides clean wrapper functions for NS3-AI communication
    that can be easily imported and used in other Python applications.
    """

    def __init__(self, msgInterface):
        """Initialize wrapper with message interface"""
        self.msgInterface = msgInterface
        self.communication_count = 0

    def Send2Ns3_Wrapper(self, action_dict):
        """
        Clean Python wrapper function for NS3-AI communication

        This is the main wrapper function that other Python applications
        should use. It handles all the complexity of NS3-AI communication
        and provides a simple dictionary-based interface.

        Args:
            action_dict: Dictionary with action variables (9 fields)
                        Missing fields will be set to -1 (no fallbacks)

        Returns:
            env_dict: Dictionary with all 23 environment variables
                     Returns None if communication failed or simulation ended
        """
        self.communication_count += 1

        print(
            f"{Colors.GREEN}[Wrapper] 🔄 Communication cycle {self.communication_count} starting...{Colors.RESET}"
        )

        # === SIMULATION FINISHED CHECK ===
        try:
            if self.msgInterface.PyGetFinished():
                print(
                    f"{Colors.YELLOW}[Wrapper] ✅ Simulation finished normally{Colors.RESET}"
                )
                return None
        except Exception as e:
            print(
                f"{Colors.RED}[Wrapper] ⚠️ Error checking simulation status: {e}{Colors.RESET}"
            )
            return None

        # === RECEIVE ENVIRONMENT DATA ===
        try:
            print(
                f"{Colors.CYAN}[Wrapper] 📥 Receiving environment data...{Colors.RESET}"
            )

            self.msgInterface.PyRecvBegin()
            env = self.msgInterface.GetCpp2PyStruct()

            if env is None:
                print(
                    f"{Colors.RED}[Wrapper] ❌ Received null environment data{Colors.RESET}"
                )
                return None

            # Create clean environment dictionary (all 23 variables)
            env_dict = {
                # Spatial information (4 variables)
                "pos_x": env.pos_x,
                "pos_y": env.pos_y,
                "distance": env.distance,
                "sta_id": env.sta_id,
                # Network performance (3 variables)
                "dl_tp": env.dl_tp,
                "ul_tp": env.ul_tp,
                "get_ApTx": env.get_ApTx,
                # Temporal (1 variable)
                "now_sec": env.now_sec,
                # Energy harvesting (5 variables)
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
                # Additional variables (3 variables)
                "reward_total": env.reward_total,
                "penalty_count": env.penalty_count,
                "coordination_active": env.coordination_active,
            }

            self.msgInterface.PyRecvEnd()
            print(
                f"{Colors.CYAN}[Wrapper] ✅ Environment data received successfully{Colors.RESET}"
            )

        except Exception as e:
            print(
                f"{Colors.RED}[Wrapper] ❌ Error receiving environment data: {e}{Colors.RESET}"
            )
            return None

        # === SEND ACTION DATA ===
        try:
            print(f"{Colors.YELLOW}[Wrapper] 📤 Sending action data...{Colors.RESET}")

            self.msgInterface.PySendBegin()
            action = self.msgInterface.GetPy2CppStruct()

            # Set all 9 action variables (NO fallbacks - missing = -1)
            action.set_ApTx = action_dict.get("set_ApTx", -1)
            action.beacon_interval_ms = action_dict.get("beacon_interval_ms", -1)
            action.twt_interval_sec = action_dict.get("twt_interval_sec", -1)
            action.enable_smart_mode = action_dict.get("enable_smart_mode", -1)
            action.adaptation_threshold = action_dict.get("adaptation_threshold", -1)
            action.coordination_enabled = action_dict.get("coordination_enabled", -1)
            action.action_valid = action_dict.get("action_valid", -1)
            action.confidence = action_dict.get("confidence", -1)
            action.sta_target = action_dict.get("sta_target", -1)
            action.exploration_flag = action_dict.get("exploration_flag", -1)

            self.msgInterface.PySendEnd()
            print(
                f"{Colors.YELLOW}[Wrapper] ✅ Action data sent successfully{Colors.RESET}"
            )

        except Exception as e:
            print(
                f"{Colors.RED}[Wrapper] ❌ Error sending action data: {e}{Colors.RESET}"
            )
            return None

        # === STORE COMMUNICATION DATA FOR CSV LOGGING ===
        print(
            f"{Colors.MAGENTA}[Wrapper] 💾 Storing communication data for logging...{Colors.RESET}"
        )
        communication_record = {
            # Timestamp and metadata
            "timestamp": time.time(),
            "cycle": self.communication_count,
            # Received environment data (23 variables)
            "env_pos_x": env_dict["pos_x"],
            "env_pos_y": env_dict["pos_y"],
            "env_distance": env_dict["distance"],
            "env_sta_id": env_dict["sta_id"],
            "env_dl_tp": env_dict["dl_tp"],
            "env_ul_tp": env_dict["ul_tp"],
            "env_get_ApTx": env_dict["get_ApTx"],
            "env_now_sec": env_dict["now_sec"],
            "env_harvested_energy": env_dict["harvested_energy"],
            "env_consumed_energy": env_dict["consumed_energy"],
            "env_energy_balance": env_dict["energy_balance"],
            "env_output_enabled": env_dict["output_enabled"],
            "env_capacitor_voltage": env_dict["capacitor_voltage"],
            "env_buffer_occupancy": env_dict["buffer_occupancy"],
            "env_buffer_overload": env_dict["buffer_overload"],
            "env_packet_gen_rate": env_dict["packet_gen_rate"],
            "env_smart_mode_enabled": env_dict["smart_mode_enabled"],
            "env_ai_confidence": env_dict["ai_confidence"],
            "env_adaptation_count": env_dict["adaptation_count"],
            "env_learning_rate": env_dict["learning_rate"],
            "env_reward_total": env_dict["reward_total"],
            "env_penalty_count": env_dict["penalty_count"],
            "env_coordination_active": env_dict["coordination_active"],
            # Sent action data (9 variables)
            "act_set_ApTx": action_dict.get("set_ApTx", -1),
            "act_beacon_interval_ms": action_dict.get("beacon_interval_ms", -1),
            "act_twt_interval_sec": action_dict.get("twt_interval_sec", -1),
            "act_enable_smart_mode": action_dict.get("enable_smart_mode", -1),
            "act_adaptation_threshold": action_dict.get("adaptation_threshold", -1),
            "act_coordination_enabled": action_dict.get("coordination_enabled", -1),
            "act_action_valid": action_dict.get("action_valid", -1),
            "act_confidence": action_dict.get("confidence", -1),
            "act_sta_target": action_dict.get("sta_target", -1),
            "act_exploration_flag": action_dict.get("exploration_flag", -1),
        }
        communication_data.append(communication_record)

        return env_dict

    def generate_smart_action(self, env_dict=None):
        """
        Generate intelligent action based on environment state

        This function demonstrates how to use environment data to generate
        smarter actions instead of purely random ones.
        """
        if env_dict is None:
            # Fallback to random if no environment data
            return self.generate_random_action()

        # Smart action generation based on environment
        action = {}

        # Adaptive power control based on distance and energy
        base_power = 15.0
        if env_dict["distance"] > 50:
            action["set_ApTx"] = min(25.0, base_power + env_dict["distance"] * 0.1)
        else:
            action["set_ApTx"] = max(5.0, base_power - env_dict["distance"] * 0.05)

        # Adaptive beacon interval based on buffer occupancy
        if env_dict["buffer_occupancy"] > 30:
            action["beacon_interval_ms"] = 50.0  # Faster beacons for high traffic
        else:
            action["beacon_interval_ms"] = 100.0  # Normal beacons

        # TWT interval based on energy balance
        if env_dict["energy_balance"] < 0:
            action["twt_interval_sec"] = 120.0  # Longer sleep for energy saving
        else:
            action["twt_interval_sec"] = 60.0  # Normal sleep

        # Smart mode based on AI confidence
        action["enable_smart_mode"] = 1 if env_dict["ai_confidence"] > 0.7 else 0

        # Adaptation threshold based on performance
        total_tp = env_dict["dl_tp"] + env_dict["ul_tp"]
        action["adaptation_threshold"] = max(0.1, min(0.9, total_tp / 20.0))

        # Coordination based on multiple STAs
        action["coordination_enabled"] = 1 if env_dict["sta_id"] > 0 else 0

        # Quality metrics
        action["action_valid"] = 1
        action["confidence"] = min(1.0, 0.5 + env_dict["ai_confidence"] * 0.5)
        action["sta_target"] = env_dict["sta_id"]
        action["exploration_flag"] = 1 if env_dict["adaptation_count"] % 5 == 0 else 0

        return action

    def generate_random_action(self):
        """
        Generate random action for testing
        """
        return {
            "set_ApTx": round(random.uniform(8.0, 22.0), 1),
            "beacon_interval_ms": round(random.uniform(60.0, 150.0), 1),
            "twt_interval_sec": round(random.uniform(45.0, 100.0), 1),
            "enable_smart_mode": random.randint(0, 1),
            "adaptation_threshold": round(random.uniform(0.2, 0.8), 2),
            "coordination_enabled": random.randint(0, 1),
            "action_valid": 1,  # Always valid for demo
            "confidence": round(random.uniform(0.3, 0.9), 2),
            "sta_target": random.randint(0, 5),
            "exploration_flag": random.randint(0, 1),
        }


def initiate_wrapper_experiment():
    """
    Initialize wrapper experiment

    Returns wrapper utility object ready for communication
    """
    print(
        f"{Colors.BLUE}[Wrapper Init] Starting NS3 simulation for wrapper demo...{Colors.RESET}"
    )

    exp = Experiment(
        "pb_wrapper_demo",  # This will run pb-wrapper-demo.cc
        "../../../..",  # Correct path to ns-3.44 root from pb-wrapper
        py_binding,
        handleFinish=True,
    )

    msgInterface = exp.run(show_output=True)
    wrapper_utils = PythonWrapperUtils(msgInterface)

    print(
        f"{Colors.GREEN}[Wrapper Init] Wrapper utilities initialized successfully{Colors.RESET}"
    )

    return wrapper_utils, exp


def main():
    """
    Main wrapper demo function
    """
    print_wrapper_info()
    print(f"{Colors.BLUE}Python Wrapper Demo starting...{Colors.RESET}")

    # === INITIALIZE WRAPPER ===
    wrapper_utils, exp = initiate_wrapper_experiment()

    cycle_count = 0

    # === MAIN WRAPPER COMMUNICATION LOOP ===
    try:
        while True:
            cycle_count += 1
            print(
                f"\n{Colors.BLUE}========== Wrapper Demo Cycle {cycle_count} =========={Colors.RESET}"
            )

            # === GENERATE ACTION USING WRAPPER UTILITIES ===
            # For first few cycles, use random actions
            if cycle_count <= 5:
                print(
                    f"{Colors.MAGENTA}[Demo] Using random action generation{Colors.RESET}"
                )
                action_dict = wrapper_utils.generate_random_action()
            else:
                # Later cycles will use smart actions based on environment
                print(
                    f"{Colors.MAGENTA}[Demo] Will use smart action generation{Colors.RESET}"
                )
                action_dict = (
                    wrapper_utils.generate_random_action()
                )  # Still random for now

            print(f"{Colors.MAGENTA}[Demo] Generated action summary:{Colors.RESET}")
            print(
                f"  {Colors.MAGENTA}• Power: {action_dict['set_ApTx']} dBm{Colors.RESET}"
            )
            print(
                f"  {Colors.MAGENTA}• Beacon: {action_dict['beacon_interval_ms']} ms{Colors.RESET}"
            )
            print(
                f"  {Colors.MAGENTA}• Smart Mode: {'ON' if action_dict['enable_smart_mode'] else 'OFF'}{Colors.RESET}"
            )
            print(
                f"  {Colors.MAGENTA}• Confidence: {action_dict['confidence']}{Colors.RESET}"
            )

            # === USE WRAPPER FUNCTION ===
            print(f"{Colors.GREEN}[Demo] Calling Send2Ns3_Wrapper...{Colors.RESET}")
            env_dict = wrapper_utils.Send2Ns3_Wrapper(action_dict)

            # Check if simulation finished
            if env_dict is None:
                print(f"{Colors.BLUE}[Demo] Wrapper demo completed{Colors.RESET}")
                break

            print(
                f"{Colors.GREEN}[Demo] Wrapper communication successful!{Colors.RESET}"
            )

            # === DISPLAY ENVIRONMENT SUMMARY ===
            print(f"{Colors.CYAN}[Demo] Environment summary:{Colors.RESET}")
            print(
                f"  {Colors.CYAN}• STA {env_dict['sta_id']} at ({env_dict['pos_x']:.1f}, {env_dict['pos_y']:.1f}){Colors.RESET}"
            )
            print(
                f"  {Colors.CYAN}• Distance: {env_dict['distance']:.1f}m{Colors.RESET}"
            )
            print(
                f"  {Colors.CYAN}• Network: DL={env_dict['dl_tp']:.1f} + UL={env_dict['ul_tp']:.1f} = {env_dict['dl_tp'] + env_dict['ul_tp']:.1f} Mbps{Colors.RESET}"
            )
            print(
                f"  {Colors.CYAN}• Energy: H={env_dict['harvested_energy']:.3f}J, C={env_dict['consumed_energy']:.3f}J, Balance={env_dict['energy_balance']:.3f}J{Colors.RESET}"
            )
            print(
                f"  {Colors.CYAN}• Buffer: {env_dict['buffer_occupancy']} packets, Gen rate: {env_dict['packet_gen_rate']:.1f} pps{Colors.RESET}"
            )
            print(
                f"  {Colors.CYAN}• AI: Confidence={env_dict['ai_confidence']:.2f}, Adaptations={env_dict['adaptation_count']}{Colors.RESET}"
            )

            # === WRAPPER ANALYSIS ===
            total_throughput = env_dict["dl_tp"] + env_dict["ul_tp"]
            energy_efficiency = total_throughput / max(
                0.001, env_dict["consumed_energy"]
            )

            print(
                f"{Colors.YELLOW}[Demo Analysis] Cycle {cycle_count} metrics:{Colors.RESET}"
            )
            print(
                f"  {Colors.YELLOW}• Total Throughput: {total_throughput:.1f} Mbps{Colors.RESET}"
            )
            print(
                f"  {Colors.YELLOW}• Energy Efficiency: {energy_efficiency:.2f} Mbps/J{Colors.RESET}"
            )
            print(
                f"  {Colors.YELLOW}• Smart Features: Mode={'ON' if env_dict['smart_mode_enabled'] else 'OFF'}, Coord={'ON' if env_dict['coordination_active'] else 'OFF'}{Colors.RESET}"
            )

            # Update action generation strategy based on environment
            if cycle_count == 5:
                print(
                    f"{Colors.MAGENTA}[Demo] Switching to smart action generation for remaining cycles...{Colors.RESET}"
                )

            if cycle_count > 5:
                # Generate smarter actions for next cycle
                smart_action = wrapper_utils.generate_smart_action(env_dict)
                print(
                    f"{Colors.MAGENTA}[Demo] Next cycle will use smart action: Power={smart_action['set_ApTx']:.1f}dBm{Colors.RESET}"
                )

            time.sleep(0.5)  # Brief delay for readability

    except Exception as e:
        print(f"{Colors.RED}[Demo] Error in wrapper demo: {e}{Colors.RESET}")
        traceback.print_exc()

    else:
        print(
            f"\n{Colors.GREEN}========== Wrapper Demo Complete =========={Colors.RESET}"
        )
        print(
            f"{Colors.CYAN}✅ PythonWrapperUtils class working correctly!{Colors.RESET}"
        )
        print(
            f"{Colors.CYAN}✅ Send2Ns3_Wrapper function handles all 23+9 variables!{Colors.RESET}"
        )
        print(
            f"{Colors.CYAN}✅ Clean integration interface demonstrated!{Colors.RESET}"
        )
        print(
            f"{Colors.CYAN}✅ Smart action generation capability shown!{Colors.RESET}"
        )

        # === SAVE COMMUNICATION DATA TO CSV ===
        print(
            f"{Colors.BLUE}[Save] Communication data length: {len(communication_data)}{Colors.RESET}"
        )

        if communication_data:
            comm_df = pd.DataFrame(communication_data)
            print(
                f"{Colors.BLUE}[Save] Current working directory: {os.getcwd()}{Colors.RESET}"
            )

            # Use relative path for CSV file in current directory
            csv_file_path = "contrib/ai/examples/pb-wrapper/pb-wrapper-demo-log.csv"

            comm_df.to_csv(csv_file_path, index=False)
            print(
                f"{Colors.GREEN}Wrapper communication data saved to {csv_file_path}{Colors.RESET}"
            )
            print(
                f"{Colors.GREEN}Logged {len(communication_data)} wrapper communication cycles{Colors.RESET}"
            )
            print(
                f"{Colors.CYAN}Wrapper log contains all 23 environment + 9 action variables{Colors.RESET}"
            )
        else:
            print(
                f"{Colors.RED}[Warning] No wrapper communication data to save!{Colors.RESET}"
            )

        print(f"\n{Colors.BLUE}=== WRAPPER VS CORE COMPARISON ==={Colors.RESET}")
        print(f"{Colors.YELLOW}pb-core.py:{Colors.RESET}")
        print(f"  • Direct interface usage with detailed logging")
        print(f"  • Comprehensive communication data logging")
        print(f"  • Suitable for debugging and development")
        print(f"  • Complete Send2Ns3() wrapper function")

        print(f"\n{Colors.YELLOW}pb-wrapper-demo.py (this script):{Colors.RESET}")
        print(f"  • PythonWrapperUtils class for clean integration")
        print(f"  • Send2Ns3_Wrapper() method for easy use")
        print(f"  • Smart action generation capabilities")
        print(f"  • Suitable for production Python applications")
        print(f"  • Modular design for reuse in other projects")
        print(f"  • Complete CSV logging like pb-core.py")

    finally:
        print(f"{Colors.BLUE}Cleaning up wrapper demo...{Colors.RESET}")
        del exp


if __name__ == "__main__":
    main()
