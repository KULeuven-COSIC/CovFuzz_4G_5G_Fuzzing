#include "shared_memory.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <signal.h>

void intHandler(int dummy) {
    SHM::keep_running = 0;
}

int main() {

    srand((unsigned)time(0));
    signal(SIGINT, intHandler);

    SHM shm(true, SHM_NAME, MSG_MAX_SIZE);

    std::unique_ptr<uint8_t[]> msg_ptr = std::make_unique<uint8_t[]>(MSG_MAX_SIZE);

    while(SHM::keep_running) {
        // std::unique_ptr<uint8_t[]> msg_ptr;
        
        uint32_t msg_size;
        if (!shm.read_bytes(1, msg_ptr.get(), msg_size)) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));

        msg_size = get_random_bytes(msg_ptr.get(), MSG_MAX_SIZE - 6);
        shm.write_bytes(1, msg_ptr.get(), msg_size);

        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));

    }

    //delete[] msg_ptr;

    return 0;
}