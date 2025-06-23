cmake_minimum_required(VERSION 3.7)

project(SHM)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fsanitize=address")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fsanitize=address -Wall")

add_library(shared_memory STATIC shared_memory.cpp shm_helper.cpp fuzz_config.cpp)

# set_target_properties(shared_memory
#     PROPERTIES
#         ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib
# )

add_executable(server server.cpp)
target_link_libraries(server shared_memory)

add_executable(client client.cpp)
target_link_libraries(client shared_memory)
