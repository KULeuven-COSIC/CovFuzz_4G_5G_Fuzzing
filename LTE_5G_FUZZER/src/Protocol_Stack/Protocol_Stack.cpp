#include "Protocol_Stack/Protocol_Stack.h"

#include "Base_Station/base_station_factory.h"
#include "Core_Network/core_network_factory.h"

#include "Configs/Fuzzing_Settings/main_config.h"
#include "my_logger.h"

#include "statistics.h"

extern Main_Config main_config_g;
extern My_Logger my_logger_g;
extern Statistics statistics_g;

Protocol_Stack::Protocol_Stack()
{
    base_station_ = Base_Station_Factory::get_base_station_by_protocol_name(main_config_g.protocol_stack_name);
    core_network_ = Core_Network_Factory::get_core_network_by_protocol_name(main_config_g.protocol_stack_name);
}

bool Protocol_Stack::start()
{
    if (!core_network_->start()) {
        my_logger_g.logger->error("Failed to start a core network");
        return false;
    }
    
    if (!base_station_->start()) {
        my_logger_g.logger->error("Failed to start a base station");
        return false;
    }

    return true;
}

bool Protocol_Stack::stop()
{
    if (!core_network_->stop()) {
        my_logger_g.logger->error("Failed to stop a core network");
        return false;
    }

    if (!base_station_->stop()) {
        my_logger_g.logger->error("Failed to stop a base station");
        return false;
    }

    return true;
}

bool Protocol_Stack::restart()
{
    if (!core_network_->restart()) {
        my_logger_g.logger->error("Failed to restart a core network");
        return false;
    }

    if (!base_station_->restart()) {
        my_logger_g.logger->error("Failed to restart a base station");
        return false;
    }

    return true;
}

bool Protocol_Stack::is_running()
{
    bool ret = true;
    if (!base_station_->is_alive()) {
        ret = false;
        statistics_g.enb_crash_counter++;
    }
    if (!core_network_->is_alive()) {
        ret = false;
        statistics_g.epc_crash_counter++;
    }
    return ret;
}
