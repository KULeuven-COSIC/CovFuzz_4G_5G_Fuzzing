#include "Protocol_Stack/Protocol_Stack_with_srsUE.h"

#include "UE/srsUE_factory.h"

#include "Configs/Fuzzing_Settings/main_config.h"
#include "my_logger.h"

#include "statistics.h"

extern Main_Config main_config_g;
extern My_Logger my_logger_g;

extern Statistics statistics_g;

Protocol_Stack_with_SRSUE::Protocol_Stack_with_SRSUE() : Protocol_Stack::Protocol_Stack()
{
    srsue_ = SRSUE_Factory::get_ue_by_protocol_name(main_config_g.protocol_stack_name);
}

bool Protocol_Stack_with_SRSUE::start()
{
    if (!Protocol_Stack::start()) return false;
    if (!srsue_->start()) {
        my_logger_g.logger->error("Failed to start a srsue");
        return false;
    }
    return true;
}

bool Protocol_Stack_with_SRSUE::stop()
{
    if (!Protocol_Stack::stop()) return false;
    if (!srsue_->stop()) {
        my_logger_g.logger->error("Failed to stop a srsue");
        return false;
    }
    return true;
}

bool Protocol_Stack_with_SRSUE::restart()
{
    if (!Protocol_Stack::restart()) return false;
    if (!srsue_->restart()) {
        my_logger_g.logger->error("Failed to restart a srsue");
        return false;
    }
    return true;
}

bool Protocol_Stack_with_SRSUE::is_running()
{
    bool ret = true;
    if (!Protocol_Stack::is_running()) {
        ret = false;
    }
    if (!srsue_->is_alive()) {
        ret = false;
        statistics_g.ue_crash_counter++;
    }
    return ret;
}
