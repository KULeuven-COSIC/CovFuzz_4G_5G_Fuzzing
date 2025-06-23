#include "Protocol_Stack/Dummy_stack.h"

#include <string>

bool Dummy_stack::start()
{
    return true;
}

bool Dummy_stack::stop()
{    
    return true;
}

bool Dummy_stack::restart()
{
    return true;
}

bool Dummy_stack::is_running()
{
    return true;
}

// PROTOCOL_TYPE Dummy_stack::get_protocol_type() {
//     return PROTOCOL_TYPE::DUMMY;
// }