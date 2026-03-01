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
 * @file pb-wrapper.h
 * @brief Wrapper class for NS3-AI Python binding integration
 *
 * Provides a clean, class-based interface for RL / AI integration
 * without requiring the verbose core test functions. Designed for
 * modularity and easy integration with existing NS-3 applications.
 */

#ifndef PB_WRAPPER_H
#define PB_WRAPPER_H

#include "pb-core.h"
#include "ns3/ns3-ai-msg-interface.h"
#include "ns3/object.h"
#include <fstream>
#include <string>

using namespace ns3;

/**
 * \brief Python Binding Wrapper for easy NS3-AI integration
 * 
 * This wrapper provides a consistent interface for Python binding integration
 * across all modules in the MobiCom energy harvesting system.
 */
class PBWrapper : public Object
{
public:
    static TypeId GetTypeId();
    
    PBWrapper();
    virtual ~PBWrapper();
    
    /**
     * \brief Initialize NS3-AI message interface
     * \return true if successfully initialized
     */
    bool Initialize();
    
    /**
     * \brief Send environment data to Python and receive actions
     * \param env_struct Environment data structure
     * \return Action data structure from Python
     */
    PBActStruct SendToPython(const PBEnvStruct& env_struct);
    
    /**
     * \brief Enable CSV logging for communication events
     * \param enable true to enable logging
     * \param logFile path to log file (optional, uses default if not provided)
     */
    void EnableLogging(bool enable, const std::string& logFile = "");
    
    /**
     * \brief Set the CSV log file path
     * \param logFile Path to CSV log file
     */
    void SetLogFile(const std::string& logFile);
    
    /**
     * \brief Get communication statistics
     * \param txCount Reference to store transmission count
     * \param rxCount Reference to store reception count
     */
    void GetStatistics(uint32_t& txCount, uint32_t& rxCount) const;
    
    /**
     * \brief Reset statistics counters
     */
    void ResetStatistics();
    
    /**
     * \brief Check if wrapper is properly initialized
     * \return true if initialized
     */
    bool IsInitialized() const;

private:
    Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>* m_msgInterface;
    bool m_initialized;
    bool m_loggingEnabled;
    std::string m_logFile;
    uint32_t m_txCount;
    uint32_t m_rxCount;
    std::ofstream m_csvLogFile;
    
    void LogEvent(const std::string& eventType, const PBEnvStruct& env, const PBActStruct& act);
};

// Legacy function wrappers for backward compatibility
Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>* GetNs3AiInterface();
PBActStruct Send2Python(Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>* msgInterface,
                        const PBEnvStruct& env_struct);

#endif // PB_WRAPPER_H