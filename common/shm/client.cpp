//#include "shared_memory.h"
#include "shm_helper.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <signal.h>

void intHandler(int dummy) {
    SHM::keep_running = 0;
}

extern SHM_Helper shm_helper_g;

int main() {

    srand((unsigned)time(0));
    signal(SIGINT, intHandler);

    // SHM shm(false, "shared_memory_test", 256);
    std::cout << "Client started" << std::endl;
    // SHM_Helper shm_helper(SHM_NAME);
    std::cout << "SHM helper created" << std::endl;

    std::unique_ptr<uint8_t[]> msg_ptr = std::make_unique<uint8_t[]>(MSG_MAX_SIZE);
    
    while(SHM::keep_running) {
        //std::unique_ptr<uint8_t[]> msg_ptr;
        
        uint32_t msg_size;

        msg_size = get_random_bytes(msg_ptr.get(), MSG_MAX_SIZE - 6);
        shm_helper_g.send_rrc_msg(0, static_cast<CHANNEL>(0), msg_ptr.get(), msg_size);
        //shm.write_bytes(0, msg_ptr, msg_size);

        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));

        // shm.read_bytes(0, msg_ptr, msg_size);

        // std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));

    }

    //delete[] msg_ptr;

    return 0;
}