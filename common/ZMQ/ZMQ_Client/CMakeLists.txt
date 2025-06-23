project(ZMQ_Client)
cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -O3")

include_directories(
	include
)

set(ZMQ_CLIENT src/zmq_client.cpp)
add_library(zmq_client_lib STATIC ${ZMQ_CLIENT})
# set_target_properties(zmq_client_lib
#     PROPERTIES
#         ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../lib
# )

# add_executable(zmq_client src/main.cpp)
# target_link_libraries(zmq_client zmq_client_lib zmq)
