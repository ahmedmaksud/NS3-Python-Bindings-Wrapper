/*
 * Copyright (c) 2025 NS3-AI Python Binding Project
 * 
 * Authors: Ahmed Maksud <amaks002@ucr.edu>
 *          SHINE Lab, Texas State University
 *          PI: Marcelo Menezes De Carvalho
 *
 * Minimal PB Wrapper for RL Integration
 * Contains only the necessary functions without main() for clean integration
 * 
 * This implementation provides robust error handling and is designed
 * for integration with existing NS-3 applications that need AI control.
 */

#include "pb-wrapper.h"
#include "pb-core.h"

#include "ns3/ai-module.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

#include <limits>
#include <fstream>
#include <iomanip>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("PBWrapper");
NS_OBJECT_ENSURE_REGISTERED(PBWrapper);

TypeId
PBWrapper::GetTypeId()
{
    static TypeId tid = TypeId("ns3::PBWrapper")
                        .SetParent<Object>()
                        .SetGroupName("Ai")
                        .AddConstructor<PBWrapper>();
    return tid;
}

PBWrapper::PBWrapper()
    : m_msgInterface(nullptr),
      m_initialized(false),
      m_loggingEnabled(false),
      m_logFile("pb-wrapper-log.csv"),
      m_txCount(0),
      m_rxCount(0)
{
    NS_LOG_FUNCTION(this);
}

PBWrapper::~PBWrapper()
{
    NS_LOG_FUNCTION(this);
    if (m_csvLogFile.is_open())
    {
        m_csvLogFile.close();
    }
}

bool
PBWrapper::Initialize()
{
    NS_LOG_FUNCTION(this);
    NS_LOG_INFO("Initializing PB Wrapper by Ahmed Maksud (SHINE Lab, Texas State University)");

    if (m_initialized)
    {
        NS_LOG_WARN("PBWrapper already initialized");
        return true;
    }

    // Use the simplified NS-3 AI interface approach
    auto interface = Ns3AiMsgInterface::Get();
    interface->SetIsMemoryCreator(false); // Python creates shared memory
    interface->SetUseVector(false);       // Use simple struct communication
    interface->SetHandleFinish(true);     // Enable proper cleanup when simulation ends

    m_msgInterface = interface->GetInterface<PBEnvStruct, PBActStruct>();
    
    if (m_msgInterface)
    {
        m_initialized = true;
        NS_LOG_INFO("PB Wrapper initialized successfully");
        return true;
    }
    else
    {
        NS_LOG_ERROR("Failed to initialize PB Wrapper");
        return false;
    }
}

PBActStruct
PBWrapper::SendToPython(const PBEnvStruct& env_struct)
{
    NS_LOG_FUNCTION(this);
    
    if (!m_initialized || !m_msgInterface)
    {
        NS_LOG_ERROR("PBWrapper: Not initialized - returning error values");
        PBActStruct error_action;
        error_action.set_ApTx = std::numeric_limits<double>::quiet_NaN();
        error_action.beacon_interval_ms = std::numeric_limits<double>::quiet_NaN();
        error_action.twt_interval_sec = std::numeric_limits<double>::quiet_NaN();
        error_action.enable_smart_mode = 0;
        error_action.adaptation_threshold = std::numeric_limits<double>::quiet_NaN();
        error_action.coordination_enabled = 0;
        error_action.action_valid = 0;
        error_action.confidence = 0.0;
        error_action.sta_target = std::numeric_limits<uint32_t>::max();
        error_action.exploration_flag = 0;
        return error_action;
    }

    try
    {
        NS_LOG_DEBUG("Sending environment data to Python");
        
        // Send environment state to Python and receive action
        m_msgInterface->CppSendBegin();
        *m_msgInterface->GetCpp2PyStruct() = env_struct;
        m_msgInterface->CppSendEnd();

        NS_LOG_DEBUG("Receiving action data from Python");
        m_msgInterface->CppRecvBegin();
        PBActStruct action = *m_msgInterface->GetPy2CppStruct();
        m_msgInterface->CppRecvEnd();

        m_txCount++;
        m_rxCount++;
        
        if (m_loggingEnabled)
        {
            LogEvent("COMMUNICATION_SUCCESS", env_struct, action);
        }

        NS_LOG_DEBUG("Communication cycle completed successfully");
        return action;
    }
    catch (const std::exception& e)
    {
        NS_LOG_ERROR("PBWrapper: Communication failed: " << e.what());
        
        PBActStruct error_action;
        error_action.set_ApTx = std::numeric_limits<double>::quiet_NaN();
        error_action.beacon_interval_ms = std::numeric_limits<double>::quiet_NaN();
        error_action.twt_interval_sec = std::numeric_limits<double>::quiet_NaN();
        error_action.enable_smart_mode = 0;
        error_action.adaptation_threshold = std::numeric_limits<double>::quiet_NaN();
        error_action.coordination_enabled = 0;
        error_action.action_valid = 0;
        error_action.confidence = 0.0;
        error_action.sta_target = std::numeric_limits<uint32_t>::max();
        error_action.exploration_flag = 0;
        
        if (m_loggingEnabled)
        {
            LogEvent("COMMUNICATION_ERROR", env_struct, error_action);
        }
        
        return error_action;
    }
}

void
PBWrapper::EnableLogging(bool enable, const std::string& logFile)
{
    NS_LOG_FUNCTION(this << enable << logFile);
    
    m_loggingEnabled = enable;
    if (!logFile.empty())
    {
        m_logFile = logFile;
    }
    
    if (enable && !m_csvLogFile.is_open())
    {
        m_csvLogFile.open(m_logFile);
        if (m_csvLogFile.is_open())
        {
            // Write CSV header
            m_csvLogFile << "Timestamp,Event_Type,TX_Count,RX_Count,Env_Value1,Env_Value2,Env_Value3,"
                         << "Act_Value1,Act_Value2,Act_Value3,Details" << std::endl;
            NS_LOG_INFO("PB Wrapper CSV logging enabled: " << m_logFile);
        }
        else
        {
            NS_LOG_ERROR("Failed to open CSV log file: " << m_logFile);
            m_loggingEnabled = false;
        }
    }
    else if (!enable && m_csvLogFile.is_open())
    {
        m_csvLogFile.close();
        NS_LOG_INFO("PB Wrapper CSV logging disabled");
    }
}

void
PBWrapper::SetLogFile(const std::string& logFile)
{
    NS_LOG_FUNCTION(this << logFile);
    m_logFile = logFile;
}

void
PBWrapper::GetStatistics(uint32_t& txCount, uint32_t& rxCount) const
{
    txCount = m_txCount;
    rxCount = m_rxCount;
}

void
PBWrapper::ResetStatistics()
{
    NS_LOG_FUNCTION(this);
    m_txCount = 0;
    m_rxCount = 0;
}

bool
PBWrapper::IsInitialized() const
{
    return m_initialized;
}

void
PBWrapper::LogEvent(const std::string& eventType, const PBEnvStruct& env, const PBActStruct& act)
{
    if (!m_csvLogFile.is_open()) return;
    
    double timestamp = Simulator::Now().GetSeconds();
    
    m_csvLogFile << std::fixed << std::setprecision(6) << timestamp << ","
                 << eventType << ","
                 << m_txCount << ","
                 << m_rxCount << ","
                 << env.sta_id << ","
                 << env.pos_x << ","
                 << env.pos_y << ","
                 << env.distance << ","
                 << env.dl_tp << ","
                 << env.ul_tp << ","
                 << env.get_ApTx << ","
                 << std::scientific << env.harvested_energy << ","
                 << std::scientific << env.consumed_energy << ","
                 << std::scientific << env.energy_balance << ","
                 << std::fixed << act.set_ApTx << ","
                 << act.beacon_interval_ms << ","
                 << act.twt_interval_sec << ","
                 << act.enable_smart_mode << ","
                 << act.confidence << ","
                 << act.action_valid << std::endl;
}

// Legacy function wrappers for backward compatibility
Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>*
GetNs3AiInterface()
{
    NS_LOG_FUNCTION_NOARGS();
    NS_LOG_INFO("Initializing NS3-AI interface by Ahmed Maksud (SHINE Lab, Texas State University)");

    // Use the simplified NS-3 AI interface approach
    auto interface = Ns3AiMsgInterface::Get();
    interface->SetIsMemoryCreator(false); // Python creates shared memory
    interface->SetUseVector(false);       // Use simple struct communication
    interface->SetHandleFinish(true);     // Enable proper cleanup when simulation ends

    auto msgInterface = interface->GetInterface<PBEnvStruct, PBActStruct>();
    
    NS_LOG_INFO("NS3-AI interface initialized successfully by Ahmed Maksud (SHINE Lab)");
    return msgInterface;
}

PBActStruct
Send2Python(ns3::Ns3AiMsgInterfaceImpl<PBEnvStruct, PBActStruct>* msgInterface,
            const PBEnvStruct& env_struct)
{
    NS_LOG_FUNCTION(msgInterface);
    
    if (!msgInterface)
    {
        NS_LOG_ERROR("PBWrapper: msgInterface is null - returning error values");
        PBActStruct error_action;
        error_action.set_ApTx = std::numeric_limits<double>::quiet_NaN();
        error_action.beacon_interval_ms = std::numeric_limits<double>::quiet_NaN();
        error_action.twt_interval_sec = std::numeric_limits<double>::quiet_NaN();
        error_action.enable_smart_mode = 0;
        error_action.adaptation_threshold = std::numeric_limits<double>::quiet_NaN();
        error_action.coordination_enabled = 0;
        error_action.action_valid = 0;
        error_action.confidence = 0.0;
        error_action.sta_target = std::numeric_limits<uint32_t>::max();
        error_action.exploration_flag = 0;
        return error_action;
    }

    try
    {
        NS_LOG_DEBUG("Sending environment data to Python");
        
        msgInterface->CppSendBegin();
        *msgInterface->GetCpp2PyStruct() = env_struct;
        msgInterface->CppSendEnd();

        NS_LOG_DEBUG("Receiving action data from Python");
        msgInterface->CppRecvBegin();
        PBActStruct action = *msgInterface->GetPy2CppStruct();
        msgInterface->CppRecvEnd();

        NS_LOG_DEBUG("Communication cycle completed successfully");
        return action;
    }
    catch (const std::exception& e)
    {
        NS_LOG_ERROR("PBWrapper: Communication failed: " << e.what());
        
        PBActStruct error_action;
        error_action.set_ApTx = std::numeric_limits<double>::quiet_NaN();
        error_action.beacon_interval_ms = std::numeric_limits<double>::quiet_NaN();
        error_action.twt_interval_sec = std::numeric_limits<double>::quiet_NaN();
        error_action.enable_smart_mode = 0;
        error_action.adaptation_threshold = std::numeric_limits<double>::quiet_NaN();
        error_action.coordination_enabled = 0;
        error_action.action_valid = 0;
        error_action.confidence = 0.0;
        error_action.sta_target = std::numeric_limits<uint32_t>::max();
        error_action.exploration_flag = 0;
        return error_action;
    }
}