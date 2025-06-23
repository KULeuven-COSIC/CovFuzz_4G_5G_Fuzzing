cmake_minimum_required(VERSION 3.7)
project(Common_Utils)

add_subdirectory(ZMQ/ZMQ_Client)
add_subdirectory(ZMQ/ZMQ_Server)
add_subdirectory(shm)
add_subdirectory(Coverage_Instrumentation)