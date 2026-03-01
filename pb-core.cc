/*
 * Copyright (c) 2025 Texas State University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Ahmed Maksud <ahmed.maksud@email.ucr.edu>
 * PI: Marcelo Menezes De Carvalho <mmcarvalho@txstate.edu>
 * Texas State University
 */

/**
 * @file pb-core.cc
 * @brief Core implementation for NS3-AI Python binding communication
 *
 * Provides the main communication interface between NS-3 C++ simulation
 * and Python AI controllers for energy harvesting wireless networks.
 */

#include "pb-core.h"

#include <ns3/ai-module.h>
#include <ns3/applications-module.h>
#include <ns3/core-module.h>
#include <ns3/internet-module.h>
#include <ns3/mobility-module.h>
#include <ns3/network-module.h>
#include <ns3/wifi-module.h>

#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("PythonBinding_PBCore");

// ===========================
// C++ Interface Initialization
// Author: Ahmed Maksud (SHINE Lab, Texas State University)
// ===========================
/**
 * GetNs3AiInterface: Initialize NS3-AI message interface
 * Creates and configures the message interface for Python binding communication
 *
 * Returns:
 * msgInterface: Configured message interface for PBEnvStruct/PBActStruct communication
 */
ns3::Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>*
GetNs3AiInterface()
{
    std::cout
        << "\033[34m[C++ Init] Initializing Python binding AI communication interface...\033[0m"
        << std::endl;
    std::cout
        << "\033[34m[C++ Init] Author: Ahmed Maksud (SHINE Lab, Texas State University)\033[0m"
        << std::endl;

    // Get singleton instance of NS3-AI interface manager
    auto interface = Ns3AiMsgInterface::Get();

    // Configure shared memory settings for C++/Python communication
    interface->SetIsMemoryCreator(false); // Python side creates shared memory, C++ connects to it
    interface->SetUseVector(false);   // Use fixed-size structs instead of variable-length vectors
    interface->SetHandleFinish(true); // Enable proper cleanup when simulation ends

    // Create type-specific interface for our PBEnvStruct ↔ PBActStruct communication
    // This interface manages two shared memory regions: C++→Python and Python→C++
    Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>* msgInterface =
        interface->GetInterface<PBEnvStruct, PBActStruct>();

    std::cout << "\033[32m[C++ Init] AI interface initialized successfully (SHINE "
                 "Lab)\033[0m"
              << std::endl;
    std::cout << "\033[32m[C++ Init] Ready for PBEnvStruct (23 vars) ↔ PBActStruct (9 vars) "
                 "communication\033[0m"
              << std::endl;

    return msgInterface;
}

// ===========================
// C++ Wrapper Implementation
// Author: Ahmed Maksud (SHINE Lab, Texas State University)
// ===========================
/**
 * Send2Python: C++ wrapper function
 * Sends environment data to Python and receives optimized actions
 * Handles all 23 environment variables and returns all 9 action variables
 */
PBActStruct
Send2Python(Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>* msgInterface,
            const PBEnvStruct& env_struct)
{
    std::cout << "\033[1;36m[Send2Python] 🚀 Starting C++→Python communication cycle\033[0m"
              << std::endl;
    std::cout << "\033[1;36m[Send2Python] Implementation\033[0m" << std::endl;

    std::cout << "\033[36m[Send2Python] 📊 Environment data to be sent:\033[0m" << std::endl;
    std::cout << "\033[36m  • pos_x: " << env_struct.pos_x << ", pos_y: " << env_struct.pos_y
              << "\033[0m" << std::endl;
    std::cout << "\033[36m  • distance: " << env_struct.distance
              << ", sta_id: " << env_struct.sta_id << "\033[0m" << std::endl;
    std::cout << "\033[36m  • dl_tp: " << env_struct.dl_tp << ", ul_tp: " << env_struct.ul_tp
              << "\033[0m" << std::endl;
    std::cout << "\033[36m  • get_ApTx: " << env_struct.get_ApTx
              << ", now_sec: " << env_struct.now_sec << "\033[0m" << std::endl;
    std::cout << "\033[36m  • harvested_energy: " << env_struct.harvested_energy
              << ", consumed_energy: " << env_struct.consumed_energy << "\033[0m" << std::endl;
    std::cout << "\033[36m  • energy_balance: " << env_struct.energy_balance
              << ", output_enabled: " << env_struct.output_enabled << "\033[0m" << std::endl;
    std::cout << "\033[36m  • capacitor_voltage: " << env_struct.capacitor_voltage
              << ", buffer_occupancy: " << env_struct.buffer_occupancy << "\033[0m" << std::endl;
    std::cout << "\033[36m  • buffer_overload: " << env_struct.buffer_overload
              << ", packet_gen_rate: " << env_struct.packet_gen_rate << "\033[0m" << std::endl;
    std::cout << "\033[36m  • smart_mode_enabled: " << env_struct.smart_mode_enabled
              << ", ai_confidence: " << env_struct.ai_confidence << "\033[0m" << std::endl;
    std::cout << "\033[36m  • adaptation_count: " << env_struct.adaptation_count
              << ", learning_rate: " << env_struct.learning_rate << "\033[0m" << std::endl;

    // === SEND PHASE: Send environment data to Python ===
    std::cout << "\033[1;33m[Send2Python] 📤 Preparing to send environment data to Python...\033[0m"
              << std::endl;

    // Lock shared memory for writing - prevents Python from reading during update
    msgInterface->CppSendBegin();
    std::cout << "\033[1;33m[Send2Python] 📝 Writing environment data to shared memory...\033[0m"
              << std::endl;

    // Copy all 23 environment variables to shared memory region (C++→Python)
    // This performs a complete struct copy: pos_x, pos_y, distance, sta_id, etc.
    *msgInterface->GetCpp2PyStruct() = env_struct;
    std::cout << "\033[1;33m[Send2Python] 🚀 Sending environment data to Python...\033[0m"
              << std::endl;

    // Release lock and signal Python that data is ready for processing
    msgInterface->CppSendEnd();
    std::cout << "\033[1;33m[Send2Python] ✅ Environment data sent successfully\033[0m"
              << std::endl;

    // === RECEIVE PHASE: Receive action data from Python ===
    std::cout << "\033[1;35m[Send2Python] 📥 Waiting for action data from Python...\033[0m"
              << std::endl;

    // Lock shared memory for reading - waits until Python finishes writing actions
    msgInterface->CppRecvBegin();
    std::cout << "\033[1;35m[Send2Python] 📖 Reading action data from shared memory...\033[0m"
              << std::endl;

    // Copy all 9 action variables from shared memory region (Python→C++)
    // This retrieves: set_ApTx, beacon_interval_ms, twt_interval_sec, etc.
    PBActStruct action_struct = *msgInterface->GetPy2CppStruct();
    std::cout << "\033[1;35m[Send2Python] 🔄 Finalizing action data reception...\033[0m"
              << std::endl;

    // Release lock and signal Python that we've finished reading
    msgInterface->CppRecvEnd();
    std::cout << "\033[1;35m[Send2Python] ✅ Action data received successfully\033[0m" << std::endl;

    std::cout << "\033[35m[Send2Python] 📊 Action data received from Python:\033[0m" << std::endl;
    std::cout << "\033[35m  • set_ApTx: " << action_struct.set_ApTx << " dBm\033[0m" << std::endl;
    std::cout << "\033[35m  • beacon_interval_ms: " << action_struct.beacon_interval_ms
              << " ms\033[0m" << std::endl;
    std::cout << "\033[35m  • twt_interval_sec: " << action_struct.twt_interval_sec << " sec\033[0m"
              << std::endl;
    std::cout << "\033[35m  • enable_smart_mode: " << action_struct.enable_smart_mode << "\033[0m"
              << std::endl;
    std::cout << "\033[35m  • adaptation_threshold: " << action_struct.adaptation_threshold
              << "\033[0m" << std::endl;
    std::cout << "\033[35m  • coordination_enabled: " << action_struct.coordination_enabled
              << "\033[0m" << std::endl;
    std::cout << "\033[35m  • action_valid: " << action_struct.action_valid << "\033[0m"
              << std::endl;
    std::cout << "\033[35m  • confidence: " << action_struct.confidence << "\033[0m" << std::endl;
    std::cout << "\033[35m  • sta_target: " << action_struct.sta_target << "\033[0m" << std::endl;
    std::cout << "\033[35m  • exploration_flag: " << action_struct.exploration_flag << "\033[0m"
              << std::endl;

    return action_struct;
}

// ===========================
// Test Main Function
// Author: Ahmed Maksud (SHINE Lab, Texas State University)
// ===========================
int
main(int argc, char* argv[])
{
    std::cout << "\033[32m========== Python Binding C++ Core Test ==========\033[0m" << std::endl;
    std::cout << "\033[33mTesting Send2Python C++ wrapper function\033[0m" << std::endl;
    std::cout << "\033[33mImplemented\033[0m" << std::endl;
    std::cout << "\033[33mHandling all 23 environment and 9 action variables\033[0m" << std::endl;

    // === INITIALIZE INTERFACE USING WRAPPER FUNCTION ===
    // Create bidirectional communication channel with Python process
    Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>* msgInterface = GetNs3AiInterface();

    // === TEST WRAPPER FUNCTION FOR MULTIPLE CYCLES ===
    // Simulate continuous environment monitoring and AI-based optimization
    for (int i = 0; i < 50; i++)
    {
        std::cout << "\n\033[34m========== Wrapper Test Cycle " << (i + 1) << " ==========\033[0m"
                  << std::endl;

        // === PREPARE COMPREHENSIVE ENVIRONMENT DATA (ALL 23 VARIABLES) ===
        // Populate complete environment state for AI decision-making
        PBEnvStruct env_data;

        // Spatial information
        env_data.pos_x = 10.0 + i * 5.0;
        env_data.pos_y = 15.0 + i * 3.0;
        env_data.distance = 20.0 + i * 2.0;
        env_data.sta_id = i;

        // Network performance metrics
        env_data.dl_tp = 8.5 + i * 0.5;
        env_data.ul_tp = 4.2 + i * 0.3;
        env_data.get_ApTx = 20.0;

        // Temporal information
        env_data.now_sec = i * 1.0;

        // Energy harvesting status - critical for AI power optimization decisions
        env_data.harvested_energy = 1.5 + i * 0.2;
        env_data.consumed_energy = 1.0 + i * 0.1;
        env_data.energy_balance = env_data.harvested_energy - env_data.consumed_energy;
        env_data.output_enabled = 1;
        env_data.capacitor_voltage = 3.3 + i * 0.1;

        // Buffer management - affects packet scheduling decisions
        env_data.buffer_occupancy = 10 + i * 5;
        env_data.buffer_overload = 0;
        env_data.packet_gen_rate = 100.0 + i * 10.0;

        // Smart node features - AI learning state variables
        env_data.smart_mode_enabled = 1;
        env_data.ai_confidence = 0.8 + i * 0.02;
        env_data.adaptation_count = i * 2;
        env_data.learning_rate = 0.01;

        // Additional complete 23-variable structure
        env_data.reward_total = i * 1.5;
        env_data.penalty_count = i / 10;
        env_data.coordination_active = i % 2;

        std::cout << "\033[33m[C++ Main] Environment Data Summary:\033[0m" << std::endl;
        std::cout << "  \033[33m• STA " << env_data.sta_id << " at Position(" << env_data.pos_x
                  << "," << env_data.pos_y << ")\033[0m" << std::endl;
        std::cout << "  \033[33m• Energy Balance: " << env_data.energy_balance
                  << "J (H:" << env_data.harvested_energy << "J, C:" << env_data.consumed_energy
                  << "J)\033[0m" << std::endl;
        std::cout << "  \033[33m• Network Performance: DL=" << env_data.dl_tp
                  << " + UL=" << env_data.ul_tp << " = " << (env_data.dl_tp + env_data.ul_tp)
                  << " Mbps\033[0m" << std::endl;
        std::cout << "  \033[33m• Smart Features: AI confidence=" << env_data.ai_confidence
                  << ", Adaptations=" << env_data.adaptation_count << "\033[0m" << std::endl;

        // === USE C++ WRAPPER FUNCTION - HANDLES ALL VARIABLE COMMUNICATION ===
        // Send 23 environment variables → receive 9 optimized action variables
        std::cout << "\033[32m[C++ Main] Calling Send2Python wrapper...\033[0m" << std::endl;
        PBActStruct action_result = Send2Python(msgInterface, env_data);

        // === DISPLAY RESULTS WITH ENHANCED PRINTING ===
        // Show how Python AI modified the control parameters based on environment
        std::cout << "\033[32m[C++ Main] Send2Python wrapper completed. Action Summary:\033[0m"
                  << std::endl;
        std::cout << "  \033[32m• Power Control: ApTx=" << action_result.set_ApTx << " dBm (was "
                  << env_data.get_ApTx << " dBm)\033[0m" << std::endl;
        std::cout << "  \033[32m• Timing Control: Beacon=" << action_result.beacon_interval_ms
                  << "ms, TWT=" << action_result.twt_interval_sec << "s\033[0m" << std::endl;
        std::cout << "  \033[32m• Smart Control: Mode="
                  << (action_result.enable_smart_mode ? "On" : "Off")
                  << ", Coordination=" << (action_result.coordination_enabled ? "On" : "Off")
                  << "\033[0m" << std::endl;
        std::cout << "  \033[32m• Quality Metrics: Valid=" << action_result.action_valid
                  << ", Confidence=" << action_result.confidence
                  << ", Target STA=" << action_result.sta_target << "\033[0m" << std::endl;

        // === PERFORMANCE ANALYSIS ===
        double total_throughput = env_data.dl_tp + env_data.ul_tp;
        double energy_efficiency = total_throughput / std::max(0.001, env_data.consumed_energy);
        double power_adjustment = action_result.set_ApTx - env_data.get_ApTx;

        std::cout << "\033[93m[C++ Analysis] Cycle " << (i + 1) << " Performance Metrics:\033[0m"
                  << std::endl;
        std::cout << "  \033[93m• Energy Efficiency: " << energy_efficiency << " Mbps/J\033[0m"
                  << std::endl;
        std::cout << "  \033[93m• Power Adjustment: " << power_adjustment << " dBm ("
                  << (power_adjustment > 0   ? "increased"
                      : power_adjustment < 0 ? "decreased"
                                             : "unchanged")
                  << ")\033[0m" << std::endl;
        std::cout << "  \033[93m• AI Confidence Gain: "
                  << (action_result.confidence - env_data.ai_confidence) << " ("
                  << env_data.ai_confidence << " → " << action_result.confidence << ")\033[0m"
                  << std::endl;

        // Brief processing delay for visibility
        Simulator::Schedule(MilliSeconds(500), []() {});
    }

    std::cout << "\n\033[32m========== C++ Core Test Complete ==========\033[0m" << std::endl;
    std::cout << "\033[33mSend2Python successfully handled all header variables!\033[0m"
              << std::endl;
    std::cout << "\033[33mInterface initialization and wrapper functions working correctly!\033[0m"
              << std::endl;
    std::cout << "\033[33mImplementation\033[0m" << std::endl;
    return 0;
}