#include "Configs/Fuzzing_Settings/serial_config.h"

#include "helpers.h"

Serial_Config serial_config_g;

std::ostream& Serial_Config::dump(std::ostream &os) const
{
    os << "Baudrate: " << baudrate << std::endl;
    os << "Modem ports: " << modem_ports << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, const Serial_Config &config)
{
    return config.dump(os);
}