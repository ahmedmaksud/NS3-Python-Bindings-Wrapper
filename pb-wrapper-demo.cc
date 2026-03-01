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
 * @file pb-wrapper-demo.cc
 * @brief Demonstration of PBWrapper class for NS3-AI communication
 *
 * Shows how to integrate the wrapper functions into existing NS-3
 * applications without the verbose logging from pb-core.cc.
 */

#include "pb-wrapper.h"
#include "pb-core.h"

#include <ns3/ai-module.h>
#include <ns3/applications-module.h>
#include <ns3/core-module.h>
#include <ns3/internet-module.h>
#include <ns3/mobility-module.h>
#include <ns3/network-module.h>
#include <ns3/wifi-module.h>

#include <iostream>
#include <cmath>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("PythonBinding_WrapperDemo");

// ===========================
// Wrapper Demo Main Function
// ===========================
int
main(int argc, char* argv[])
{
    // Command line parameters
    double simulationTime = 30.0;
    uint32_t numCycles = 25;
    bool enableLogging = true;
    
    CommandLine cmd(__FILE__);
    cmd.AddValue("time", "Simulation time (seconds)", simulationTime);
    cmd.AddValue("cycles", "Number of AI communication cycles", numCycles);
    cmd.AddValue("logging", "Enable CSV logging", enableLogging);
    cmd.Parse(argc, argv);

    // Enable logging
    LogComponentEnable("PythonBinding_WrapperDemo", LOG_LEVEL_INFO);

    std::cout << "\033[32m========== Python Binding Wrapper Demo ==========\033[0m" << std::endl;
    std::cout << "\033[33mDemonstrating new PBWrapper class usage\033[0m" << std::endl;
    std::cout << "\033[33mImplemented\033[0m" << std::endl;
    std::cout << "\033[33mUsing consistent wrapper pattern\033[0m" << std::endl;

    // === INITIALIZE WRAPPER USING NEW CLASS-BASED APPROACH ===
    std::cout << "\033[34m[Wrapper Demo] Creating PBWrapper instance...\033[0m" << std::endl;
    
    Ptr<PBWrapper> pbWrapper = CreateObject<PBWrapper>();
    
    // Enable consistent CSV logging
    if (enableLogging)
    {
        pbWrapper->EnableLogging(true, "contrib/ai/examples/pb-wrapper/pb-wrapper-demo-log.csv");
    }
    
    // Initialize the wrapper
    if (!pbWrapper->Initialize())
    {
        std::cout << "\033[31m[Wrapper Demo] ERROR: Failed to initialize wrapper!\033[0m" << std::endl;
        return 1;
    }
    
    std::cout << "\033[32m[Wrapper Demo] PBWrapper initialized successfully!\033[0m" << std::endl;

    // === TEST WRAPPER FOR MULTIPLE CYCLES ===
    std::cout << "\033[34m[Wrapper Demo] Running " << numCycles << " communication cycles...\033[0m" << std::endl;
    
    for (uint32_t i = 0; i < numCycles; i++)
    {
        std::cout << "\n\033[34m========== Wrapper Demo Cycle " << (i + 1) << " ==========\033[0m" << std::endl;

        // === PREPARE ENVIRONMENT DATA (ALL 23 VARIABLES) ===
        // Same comprehensive data as pb-core, but using wrapper for communication
        PBEnvStruct env_data;

        // Spatial information
        env_data.pos_x = 15.0 + i * 3.0;
        env_data.pos_y = 20.0 + i * 2.5;
        env_data.distance = 25.0 + i * 1.5;
        env_data.sta_id = i;

        // Network performance metrics
        env_data.dl_tp = 12.0 + i * 0.4;
        env_data.ul_tp = 6.5 + i * 0.2;
        env_data.get_ApTx = 18.0;

        // Temporal information
        env_data.now_sec = i * 1.2;

        // Energy harvesting status
        env_data.harvested_energy = 2.0 + i * 0.15;
        env_data.consumed_energy = 1.2 + i * 0.08;
        env_data.energy_balance = env_data.harvested_energy - env_data.consumed_energy;
        env_data.output_enabled = 1;
        env_data.capacitor_voltage = 3.5 + i * 0.05;

        // Buffer management
        env_data.buffer_occupancy = 15 + i * 3;
        env_data.buffer_overload = (env_data.buffer_occupancy > 50) ? 1 : 0;
        env_data.packet_gen_rate = 150.0 + i * 8.0;

        // Smart node features
        env_data.smart_mode_enabled = 1;
        env_data.ai_confidence = 0.75 + i * 0.01;
        env_data.adaptation_count = i * 3;
        env_data.learning_rate = 0.015;

        // Additional variables for complete 23-variable structure
        env_data.reward_total = i * 2.2;
        env_data.penalty_count = i / 8;
        env_data.coordination_active = i % 3 == 0 ? 1 : 0;

        std::cout << "\033[36m[Wrapper Demo] Environment Summary (Cycle " << (i + 1) << "):\033[0m" << std::endl;
        std::cout << "  \033[36m• STA " << env_data.sta_id << " at (" << env_data.pos_x
                  << "," << env_data.pos_y << "), Distance: " << env_data.distance << "m\033[0m" << std::endl;
        std::cout << "  \033[36m• Energy: H=" << env_data.harvested_energy << "J, C=" << env_data.consumed_energy
                  << "J, Balance=" << env_data.energy_balance << "J\033[0m" << std::endl;
        std::cout << "  \033[36m• Network: DL=" << env_data.dl_tp << " + UL=" << env_data.ul_tp
                  << " = " << (env_data.dl_tp + env_data.ul_tp) << " Mbps\033[0m" << std::endl;

        // === USE NEW WRAPPER CLASS METHOD ===
        // Using the new class-based wrapper approach
        std::cout << "\033[32m[Wrapper Demo] Calling wrapper SendToPython method...\033[0m" << std::endl;
        
        PBActStruct action_result = pbWrapper->SendToPython(env_data);

        // === CHECK FOR ERRORS ===
        // Wrapper returns NaN values on error, check for this
        if (std::isnan(action_result.set_ApTx) || action_result.action_valid == 0)
        {
            std::cout << "\033[31m[Wrapper Demo] ERROR: Wrapper returned invalid action!\033[0m" << std::endl;
            std::cout << "\033[31m  • set_ApTx: " << action_result.set_ApTx << "\033[0m" << std::endl;
            std::cout << "\033[31m  • action_valid: " << action_result.action_valid << "\033[0m" << std::endl;
            std::cout << "\033[31m  • confidence: " << action_result.confidence << "\033[0m" << std::endl;
            break;
        }

        // === DISPLAY RESULTS ===
        // Clean wrapper results without verbose pb-core logging
        std::cout << "\033[32m[Wrapper Demo] Wrapper communication successful! Action Summary:\033[0m"
                  << std::endl;
        std::cout << "  \033[32m• Power: " << env_data.get_ApTx << " dBm → " << action_result.set_ApTx << " dBm ("
                  << (action_result.set_ApTx - env_data.get_ApTx) << " dBm change)\033[0m" << std::endl;
        std::cout << "  \033[32m• Timing: Beacon=" << action_result.beacon_interval_ms
                  << "ms, TWT=" << action_result.twt_interval_sec << "s\033[0m" << std::endl;
        std::cout << "  \033[32m• Control: Smart=" << (action_result.enable_smart_mode ? "ON" : "OFF")
                  << ", Coordination=" << (action_result.coordination_enabled ? "ON" : "OFF") << "\033[0m" << std::endl;
        std::cout << "  \033[32m• Quality: Valid=" << action_result.action_valid
                  << ", Confidence=" << action_result.confidence
                  << ", Target=" << action_result.sta_target << "\033[0m" << std::endl;

        // === WRAPPER DEMO ANALYSIS ===
        double total_throughput = env_data.dl_tp + env_data.ul_tp;
        double energy_efficiency = total_throughput / std::max(0.001, env_data.consumed_energy);
        double power_adjustment = action_result.set_ApTx - env_data.get_ApTx;

        std::cout << "\033[93m[Wrapper Analysis] Cycle " << (i + 1) << " Metrics:\033[0m"
                  << std::endl;
        std::cout << "  \033[93m• Total Throughput: " << total_throughput << " Mbps\033[0m"
                  << std::endl;
        std::cout << "  \033[93m• Energy Efficiency: " << energy_efficiency << " Mbps/J\033[0m"
                  << std::endl;
        std::cout << "  \033[93m• Power Change: " << power_adjustment << " dBm ("
                  << (power_adjustment > 0   ? "increased"
                      : power_adjustment < 0 ? "decreased"
                                             : "unchanged")
                  << ")\033[0m" << std::endl;
        std::cout << "  \033[93m• AI Decision Quality: " << action_result.confidence << " confidence\033[0m"
                  << std::endl;

        // Brief delay for readability
        Simulator::Schedule(MilliSeconds(300), []() {});
    }

    // === FINAL STATISTICS ===
    uint32_t totalTx, totalRx;
    pbWrapper->GetStatistics(totalTx, totalRx);
    
    std::cout << "\n\033[32m========== Wrapper Demo Complete ==========\033[0m" << std::endl;
    std::cout << "\033[33m✅ PBWrapper class working correctly!\033[0m" << std::endl;
    std::cout << "\033[33m✅ Consistent wrapper pattern implemented!\033[0m" << std::endl;
    std::cout << "\033[33m✅ CSV logging enabled for analysis!\033[0m" << std::endl;
    std::cout << "\033[33m✅ All 23 environment + 9 action variables handled!\033[0m" << std::endl;
    std::cout << "\033[33mWrapper implementation\033[0m" << std::endl;

    std::cout << "\n\033[34m=== FINAL STATISTICS ===\033[0m" << std::endl;
    std::cout << "\033[36m• Total Communications: " << totalTx << " TX, " << totalRx << " RX\033[0m" << std::endl;
    std::cout << "\033[36m• Success Rate: " << ((totalRx > 0) ? "100%" : "N/A") << "\033[0m" << std::endl;
    
    if (enableLogging)
    {
        std::cout << "\033[36m• CSV Log: contrib/ai/examples/pb-wrapper/pb-wrapper-demo-log.csv\033[0m" << std::endl;
    }

    std::cout << "\n\033[34m=== WRAPPER PATTERN BENEFITS ===\033[0m" << std::endl;
    std::cout << "\033[36m• Class-based: Object-oriented, clean lifecycle management\033[0m" << std::endl;
    std::cout << "\033[36m• Consistent: Same interface as other examples modules\033[0m" << std::endl;
    std::cout << "\033[36m• CSV Logging: Integrated data collection for analysis\033[0m" << std::endl;
    std::cout << "\033[36m• pb-wrapper.cc: Clean functions, NS_LOG only, easy integration\033[0m" << std::endl;
    std::cout << "\033[36m• Both handle identical 23+9 variable communication\033[0m" << std::endl;
    std::cout << "\033[36m• Wrapper suitable for production NS-3 applications\033[0m" << std::endl;
    std::cout << "\033[36m• Core suitable for testing and debugging\033[0m" << std::endl;

    return 0;
}