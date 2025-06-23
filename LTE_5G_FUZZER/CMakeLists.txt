cmake_minimum_required(VERSION 3.7)

project(LTE_5G_Fuzzer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fsanitize=address -pedantic-errors -Wall -Werror")

set(THIRD_PARTY_PATH $ENV{THIRD_PARTY_PATH})
message("Set THIRD_PARTY_PATH to ${THIRD_PARTY_PATH}")
set(COMMON_INCLUDE_PATH $ENV{COMMON_INCLUDE_PATH})
message("Set COMMON_INCLUDE_PATH to ${COMMON_INCLUDE_PATH}")
set(COMMON_BUILD_PATH $ENV{COMMON_BUILD_PATH})
message("Set COMMON_BUILD_PATH to ${COMMON_BUILD_PATH}")

include_directories(
	include
	${COMMON_INCLUDE_PATH}
	${COMMON_INCLUDE_PATH}/shm/
	${COMMON_INCLUDE_PATH}/ZMQ/ZMQ_Client/include
	${COMMON_INCLUDE_PATH}/ZMQ/ZMQ_Server/include
	${THIRD_PARTY_PATH}/WDissector
	${THIRD_PARTY_PATH}/wireshark
	/usr/include/glib-2.0
	/usr/lib/x86_64-linux-gnu/glib-2.0/include
)

link_directories(
	${COMMON_BUILD_PATH}/shm
	${COMMON_BUILD_PATH}/ZMQ/ZMQ_Client
	${COMMON_BUILD_PATH}/ZMQ/ZMQ_Server
	${THIRD_PARTY_PATH}/WDissector/build
)

add_executable(LTE_5G_Fuzzer
	src/Base_Station/base_station_factory.cpp
	src/Base_Station/srsGNB.cpp
	src/Base_Station/srsGNB_base.cpp
	src/Base_Station/srsGNB_ZMQ.cpp
	src/Base_Station/srsENB.cpp
	src/Base_Station/srsENB_base.cpp
	src/Base_Station/srsENB_ZMQ.cpp
	src/Communication/shm_layer_communication.cpp
	src/Communication/shm_layer_communication_factory.cpp
	src/Communication/shm_MAC_communication.cpp
	src/Communication/shm_RRC_communication.cpp
	src/Communication/srsue_communication.cpp
	src/Configs/Fuzzing_Strategies/fuzz_strategy_config.cpp
	src/Configs/Fuzzing_Settings/main_config.cpp
	src/Configs/Fuzzing_Settings/serial_config.cpp
	src/Configs/Fuzzing_Settings/srsran_config.cpp
	src/Configs/Fuzzing_Settings/timers_config.cpp
	src/Coordinators/active_coordinator.cpp
	src/Coordinators/base_coordinator.cpp
	src/Coordinators/dummy_coordinator.cpp
	src/Coordinators/srs_coordinator.cpp
	src/Coordinators/coordinator_factory.cpp
	src/Core_Network/core_network_factory.cpp
	src/Core_Network/Open5Gs.cpp
	src/Core_Network/srsEPC.cpp
	src/Fuzzers/base_fuzzer.cpp
	src/Fuzzers/random_fuzzer.cpp
	src/Fuzzers/dummy_fuzzer.cpp
	src/Fuzzers/fuzzer_factory.cpp
	src/Protocol_Stack/Protocol_Stack.cpp
	src/Protocol_Stack/Protocol_Stack_with_srsUE.cpp
	src/Protocol_Stack/Dummy_stack.cpp
	src/Protocol_Stack/Protocol_type.cpp
	src/Protocol_Stack/protocol_stack_factory.cpp
	src/UE/Android_UE.cpp
	src/UE/AT_Modem.cpp
	src/UE/Dummy_UE.cpp
	src/UE/srsUE.cpp
	src/UE/srsUE_base.cpp
	src/UE/srsUE_factory.cpp
	src/UE/srsUE_ZMQ.cpp
	src/UE/UE_factory.cpp
	src/UE/Communication/AT_Serial_Communication.cpp
	src/coverage_tracker.cpp
	src/coverage_requester.cpp
	src/dissector.cpp
	src/field.cpp
	src/helpers.cpp
	src/my_logger.cpp
	src/main.cpp
	src/mutation.cpp
	src/mutator.cpp
	src/mutated_field_num_tracker.cpp
	src/packet.cpp
	src/parsed_field.cpp
	src/patch.cpp
	src/seed.cpp
	src/statistics.cpp
)

target_link_libraries(LTE_5G_Fuzzer fmt shared_memory wdissector zmq zmq_client_lib zmq_server_lib serial)
