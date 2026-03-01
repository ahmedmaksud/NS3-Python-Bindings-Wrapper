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
 * @file pb-core.h
 * @brief Core data structures for NS3-AI Python binding communication
 *
 * Defines the shared data structures used for bidirectional communication
 * between NS-3 C++ simulation and Python AI controllers in energy
 * harvesting wireless networks.
 *
 * Key components:
 * - PBEnvStruct: 23 environment variables describing network state
 * - PBActStruct: 9 action variables for network optimization
 * - Communication helper function declarations
 *
 * Communication Flow:
 * C++:    action = Send2Python(msgInterface, env)   [send env, receive action]
 * Python: env    = Send2Ns3(action)                 [send action, receive env]
 */

#ifndef PB_CORE_H
#define PB_CORE_H

// Include standard integer types for struct field definitions
#include <cstdint>

// Forward declaration for ns3-ai message interface template class
// This avoids including the full ns3-ai headers in this header file
// The template parameters (EnvType, ActType) will be our PBEnvStruct and PBActStruct
namespace ns3
{
template <typename EnvType, typename ActType>
class Ns3AiMsgInterfaceImpl;
}

/**
 * PBEnvStruct: Environment Data Structure (23 variables)
 *
 * Contains comprehensive information about the current network state
 * including spatial information, network metrics, energy harvesting status,
 * and smart node coordination features.
 *
 * Compatible with ns3-ai for efficient shared memory communication.
 *
 */
struct PBEnvStruct
{
    // === SPATIAL INFORMATION (4 variables) ===
    double pos_x;    // Node X position in meters
    double pos_y;    // Node Y position in meters
    double distance; // Distance to access point in meters
    uint32_t sta_id; // Station identifier

    // === NETWORK PERFORMANCE METRICS (3 variables) ===
    double dl_tp;    // Downlink throughput in Mbps
    double ul_tp;    // Uplink throughput in Mbps
    double get_ApTx; // Access point transmission power in dBm

    // === TEMPORAL INFORMATION (1 variable) ===
    double now_sec; // Current simulation time in seconds

    // === ENERGY HARVESTING STATUS (5 variables) ===
    double harvested_energy;  // Energy harvested from RF signals in Joules
    double consumed_energy;   // Energy consumed by operations in Joules
    double energy_balance;    // Net energy balance (harvested - consumed) in Joules
    uint32_t output_enabled;  // Output capability status (0/1)
    double capacitor_voltage; // Energy storage capacitor voltage in Volts

    // === BUFFER MANAGEMENT (3 variables) ===
    uint32_t buffer_occupancy; // Communication buffer occupancy percentage
    uint32_t buffer_overload;  // Buffer overload indicator (0/1)
    double packet_gen_rate;    // Packet generation rate in packets/second

    // === SMART NODE FEATURES - ENERGY HARVESTING SPECIFIC (4 variables) ===
    uint32_t smart_mode_enabled; // Smart coordination mode status (0/1)
    double ai_confidence;        // AI decision confidence level (0.0-1.0)
    uint32_t adaptation_count;   // Number of adaptations performed
    double learning_rate;        // Current learning rate for adaptation

    // Additional variables for complete 23-variable structure
    double reward_total;          // Total accumulated reward
    uint32_t penalty_count;       // Number of penalties incurred
    uint32_t coordination_active; // Inter-node coordination active status (0/1)
};

/**
 * PBActStruct: Action Data Structure (9 variables)
 *
 * Contains optimization actions determined by the AI controller
 * for network parameter adjustment and energy management.
 *
 * All variables should be set by AI controller; use -1 for unavailable values.
 *
 */
struct PBActStruct
{
    // === PRIMARY CONTROL ACTIONS (3 variables) ===
    double set_ApTx;           // Target access point transmission power in dBm
    double beacon_interval_ms; // Beacon transmission interval in milliseconds
    double twt_interval_sec;   // Target wake time interval in seconds

    // === SMART NODE CONTROL - ENERGY HARVESTING SPECIFIC (3 variables) ===
    uint32_t enable_smart_mode;    // Enable/disable smart coordination (0/1)
    double adaptation_threshold;   // Threshold for triggering adaptations
    uint32_t coordination_enabled; // Enable/disable inter-node coordination (0/1)

    // === ACTION METADATA (3 variables) ===
    uint32_t action_valid;     // Action validity indicator (0/1)
    double confidence;         // AI confidence in this action (0.0-1.0)
    uint32_t sta_target;       // Target station for directed actions
    uint32_t exploration_flag; // Exploration vs exploitation indicator (0/1)
};

/**
 * C++ interface initialization function
 * Creates and configures the NS3-AI message interface for Python binding communication
 *
 * Returns:
 *   msgInterface: Configured message interface for PBEnvStruct/PBActStruct communication
 */
ns3::Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>* GetNs3AiInterface();

/**
 * C++ wrapper function: Send environment to Python and receive action
 *
 * @param msgInterface: NS-3 AI message interface
 * @param env_struct: Environment data to send to Python (all 23 variables)
 * @return action_struct: Optimized actions received from Python (all 9 variables)
 */
PBActStruct Send2Python(ns3::Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>* msgInterface,
                        const PBEnvStruct& env_struct);

#endif // PB_CORE_H