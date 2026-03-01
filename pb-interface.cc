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
 * @file pb-interface.cc
 * @brief Python bindings (pybind11) for PBEnvStruct and PBActStruct
 *
 * Creates Python-accessible versions of the shared data structures,
 * enabling seamless NS3-AI communication between C++ and Python.
 */

#include "pb-core.h"

#include <ns3/ai-module.h>

#include <iostream>
#include <pybind11/pybind11.h>

namespace py = pybind11;

/**
 * Python Binding Module: pb-interface
 *
 * Creates Python-accessible versions of:
 * - PBEnvStruct (23 environment variables)
 * - PBActStruct (9 action variables)
 * - Ns3AiMsgInterfaceImpl (communication interface)
 */
PYBIND11_MODULE(pb_interface_py, m)
{
    m.doc() = "Python Binding Interface for NS3-AI Communication";

    // Environment Structure (23 variables)
    py::class_<PBEnvStruct>(m, "PBEnvStruct")
        .def(py::init<>(), "Initialize environment structure")
        // Spatial information (4 variables)
        .def_readwrite("pos_x", &PBEnvStruct::pos_x, "Node X position in meters")
        .def_readwrite("pos_y", &PBEnvStruct::pos_y, "Node Y position in meters")
        .def_readwrite("distance", &PBEnvStruct::distance, "Distance to access point in meters")
        .def_readwrite("sta_id", &PBEnvStruct::sta_id, "Station identifier")
        // Network performance metrics (3 variables)
        .def_readwrite("dl_tp", &PBEnvStruct::dl_tp, "Downlink throughput in Mbps")
        .def_readwrite("ul_tp", &PBEnvStruct::ul_tp, "Uplink throughput in Mbps")
        .def_readwrite("get_ApTx", &PBEnvStruct::get_ApTx, "Access point transmission power in dBm")
        // Temporal information (1 variable)
        .def_readwrite("now_sec", &PBEnvStruct::now_sec, "Current simulation time in seconds")
        // Energy harvesting status (5 variables)
        .def_readwrite("harvested_energy",
                       &PBEnvStruct::harvested_energy,
                       "Energy harvested from RF signals in Joules")
        .def_readwrite("consumed_energy",
                       &PBEnvStruct::consumed_energy,
                       "Energy consumed by operations in Joules")
        .def_readwrite("energy_balance",
                       &PBEnvStruct::energy_balance,
                       "Net energy balance (harvested - consumed) in Joules")
        .def_readwrite("output_enabled",
                       &PBEnvStruct::output_enabled,
                       "Output capability status (0/1)")
        .def_readwrite("capacitor_voltage",
                       &PBEnvStruct::capacitor_voltage,
                       "Energy storage capacitor voltage in Volts")
        // Buffer management (3 variables)
        .def_readwrite("buffer_occupancy",
                       &PBEnvStruct::buffer_occupancy,
                       "Communication buffer occupancy percentage")
        .def_readwrite("buffer_overload",
                       &PBEnvStruct::buffer_overload,
                       "Buffer overload indicator (0/1)")
        .def_readwrite("packet_gen_rate",
                       &PBEnvStruct::packet_gen_rate,
                       "Packet generation rate in packets/second")
        // Smart node features (4 variables)
        .def_readwrite("smart_mode_enabled",
                       &PBEnvStruct::smart_mode_enabled,
                       "Smart coordination mode status (0/1)")
        .def_readwrite("ai_confidence",
                       &PBEnvStruct::ai_confidence,
                       "AI decision confidence level (0.0-1.0)")
        .def_readwrite("adaptation_count",
                       &PBEnvStruct::adaptation_count,
                       "Number of adaptations performed")
        .def_readwrite("learning_rate",
                       &PBEnvStruct::learning_rate,
                       "Current learning rate for adaptation")
        // Additional variables for complete 23-variable structure (3 variables)
        .def_readwrite("reward_total", &PBEnvStruct::reward_total, "Total accumulated reward")
        .def_readwrite("penalty_count", &PBEnvStruct::penalty_count, "Number of penalties incurred")
        .def_readwrite("coordination_active",
                       &PBEnvStruct::coordination_active,
                       "Inter-node coordination active status (0/1)")
        .def("__repr__", [](const PBEnvStruct& env) {
            return "<PBEnvStruct: STA " + std::to_string(env.sta_id) + " at (" +
                   std::to_string(env.pos_x) + ", " + std::to_string(env.pos_y) + ")>";
        });

    // Action Structure (9 variables)
    py::class_<PBActStruct>(m, "PBActStruct")
        .def(py::init<>(), "Initialize action structure")
        // Primary control actions (3 variables)
        .def_readwrite("set_ApTx",
                       &PBActStruct::set_ApTx,
                       "Target access point transmission power in dBm")
        .def_readwrite("beacon_interval_ms",
                       &PBActStruct::beacon_interval_ms,
                       "Beacon transmission interval in milliseconds")
        .def_readwrite("twt_interval_sec",
                       &PBActStruct::twt_interval_sec,
                       "Target wake time interval in seconds")
        // Smart node control (3 variables)
        .def_readwrite("enable_smart_mode",
                       &PBActStruct::enable_smart_mode,
                       "Enable/disable smart coordination (0/1)")
        .def_readwrite("adaptation_threshold",
                       &PBActStruct::adaptation_threshold,
                       "Threshold for triggering adaptations")
        .def_readwrite("coordination_enabled",
                       &PBActStruct::coordination_enabled,
                       "Enable/disable inter-node coordination (0/1)")
        // Action metadata (3 variables)
        .def_readwrite("action_valid",
                       &PBActStruct::action_valid,
                       "Action validity indicator (0/1)")
        .def_readwrite("confidence",
                       &PBActStruct::confidence,
                       "AI confidence in this action (0.0-1.0)")
        .def_readwrite("sta_target",
                       &PBActStruct::sta_target,
                       "Target station for directed actions")
        .def_readwrite("exploration_flag",
                       &PBActStruct::exploration_flag,
                       "Exploration vs exploitation indicator (0/1)")
        .def("__repr__", [](const PBActStruct& act) {
            return "<PBActStruct: ApTx=" + std::to_string(act.set_ApTx) +
                   "dBm, valid=" + std::to_string(act.action_valid) + ">";
        });

    // NS3-AI Message Interface
    py::class_<ns3::Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>>(m, "Ns3AiMsgInterfaceImpl")
        .def(py::init<bool,
                      bool,
                      bool,
                      uint32_t,
                      const char*,
                      const char*,
                      const char*,
                      const char*>(),
             "Initialize message interface")
        // Communication control methods
        .def("PyRecvBegin",
             &ns3::Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>::PyRecvBegin,
             "Begin receiving data from C++")
        .def("PyRecvEnd",
             &ns3::Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>::PyRecvEnd,
             "End receiving data from C++")
        .def("PySendBegin",
             &ns3::Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>::PySendBegin,
             "Begin sending data to C++")
        .def("PySendEnd",
             &ns3::Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>::PySendEnd,
             "End sending data to C++")
        .def("PyGetFinished",
             &ns3::Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>::PyGetFinished,
             "Check if simulation finished")
        // Data access methods
        .def("GetCpp2PyStruct",
             &ns3::Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>::GetCpp2PyStruct,
             py::return_value_policy::reference,
             "Get environment data from C++")
        .def("GetPy2CppStruct",
             &ns3::Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>::GetPy2CppStruct,
             py::return_value_policy::reference,
             "Get action structure to send to C++");

    // Module metadata

    m.attr("__email__") = "amaks002@ucr.edu";
    m.attr("__lab__") = "SHINE Lab, Texas State University";
    m.attr("__pi__") = "Marcelo Menezes De Carvalho";
    m.attr("__version__") = "1.0.0";
    m.attr("__description__") = "NS3-AI Python Binding Interface for Energy Harvesting Networks";
}