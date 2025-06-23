project(Instrumentation)
cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -O3")

include_directories(include)

set(INSTR src/instrumentation.cpp)
add_library(instr STATIC ${INSTR})
# set_target_properties(instr
#     PROPERTIES
#         ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib
# )