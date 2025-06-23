project(ZMQ_Server)
cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -O3")

include_directories(
	include
)

set(ZMQ_SERVER src/zmq_server.cpp)
add_library(zmq_server_lib STATIC ${ZMQ_SERVER})
# set_target_properties(zmq_server_lib
#     PROPERTIES
#         ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../lib
# )

# add_executable(zmq_server src/main.cpp)
# target_link_libraries(zmq_server zmq_server_lib zmq)
