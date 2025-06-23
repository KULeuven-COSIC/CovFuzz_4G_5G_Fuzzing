#include "Core_Network/Open5Gs.h"

#include "Configs/Fuzzing_Settings/srsran_config.h"
#include "my_logger.h"

#include <chrono>
#include <cstdlib>
#include <string>
#include <thread>

extern My_Logger my_logger_g;
extern SRSRAN_Config srsran_config_g;

bool Open5Gs::start()
{
    // std::string command = "gnome-terminal --geometry=80x24+150+50 --title=Open5Gs -- bash -c \"cd " + srsran_config_g.path_to_open5gs + "; ./start_open5gs.sh" + 
    //         (srsran_config_g.close_core_network_after_fuzzing ? "" : ";bash") + "\"";
    // if (system(command.c_str()) != 0) {
    //     return false;
    // }
    return true;
}

bool Open5Gs::stop()
{
    // std::string command = srsran_config_g.path_to_open5gs + "/kill_open5gs.sh";
    // if (system(command.c_str()) != 0) {
    //     return false;
    // }
    return true;
}

bool Open5Gs::restart()
{
    // if (!stop()) {
    //     my_logger_g.logger->error("Failed to stop {}", name_);
    //     return false;
    // }

    // std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // if (!start()) {
    //     my_logger_g.logger->error("Failed to start {}", name_);
    //     return false; 
    // }
    return true;
}

bool Open5Gs::is_alive()
{
    // TODO: Implement
    return true;
}
