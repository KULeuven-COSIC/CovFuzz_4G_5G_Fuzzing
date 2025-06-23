#include "UE/Android_UE.h"

#include "UE/Communication/AT_Serial_Communication.h"

#include "Configs/Fuzzing_Settings/main_config.h"
#include "Configs/Fuzzing_Settings/timers_config.h"

#include "helpers.h"
#include "my_logger.h"

#include <chrono>
#include <thread>

extern My_Logger my_logger_g;
extern Main_Config main_config_g;
extern Timers_Config timers_config_g;

bool Android_UE::start() {
    return true;
}

bool Android_UE::stop() {
    return true;
}

bool Android_UE::restart() {
    // TODO: Add restart support
    return true;
}

bool Android_UE::is_alive() {
    // TODO: Add automatic modem alive check. So far, to detect crashes, we manually use the logcat tool (https://developer.android.com/tools/logcat) after each fuzzing campaign.
    return true;
}

bool Android_UE::is_connected() {
    std::string command = "adb shell dumpsys telephony.registry | grep mServiceState | grep mAlphaShort=srsRAN | grep 'mVoiceRegState=0(IN_SERVICE)' > /dev/null";
    for (int i = 0; i < 3; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        my_logger_g.logger->debug("Writing to shell: {}", command);
        if (system(command.c_str()) == 0) return true;
    }
    return false;
}

bool Android_UE::connect_to_network() {
    if (!set_ue_to_full_functionality()) return false;
    std::this_thread::sleep_for(std::chrono::seconds(timers_config_g.AT_CONNECT_TO_NETWORK));
    return true;
}

bool Android_UE::disconnect_from_network() {
    return set_to_airplane_mode();
}

bool Android_UE::set_to_airplane_mode() {
    std::string command = "adb shell cmd connectivity airplane-mode enable";
    my_logger_g.logger->debug("Writing to shell: {}", command);
    if (system(command.c_str()) != 0) {
        my_logger_g.logger->error("Could not enable airplane mode via ADB");
        return false;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return true;
}

bool Android_UE::set_ue_to_full_functionality() {
    std::string command = "adb shell cmd connectivity airplane-mode disable";
    my_logger_g.logger->debug("Writing to shell: {}", command);
    if (system(command.c_str()) != 0) {
        my_logger_g.logger->error("Could not disable airplane mode via ADB");
        return false;
    }
    return true;
}