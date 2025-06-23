#
# Copyright 2021-2023 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

# FUZZ
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize-coverage=edge,no-prune,trace-pc-guard")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize-coverage=edge,no-prune,trace-pc-guard")

#set(COMMON_PATH /home/anonimous/Projects/CovFuzz_EuroSP_2025/common)

include_directories(${COMMON_INCLUDE_PATH}/shm)
link_directories(${COMMON_BUILD_PATH}/shm)

include_directories(${COMMON_INCLUDE_PATH}/Coverage_Instrumentation/include)
link_directories(${COMMON_BUILD_PATH}/Coverage_Instrumentation)

include_directories(${COMMON_INCLUDE_PATH}/ZMQ/ZMQ_Server/include)
link_directories(${COMMON_BUILD_PATH}/ZMQ/ZMQ_Server)

include_directories(coverage_listener)

# FUZZ

add_executable(gnb
        gnb.cpp
        gnb_appconfig_cli11_schema.cpp
        gnb_appconfig_validators.cpp
        gnb_appconfig_translators.cpp
        gnb_worker_manager.cpp
        gnb_e2_metric_connector_manager.cpp
        gnb_du_factory.cpp
        helpers/metrics_plotter_stdout.cpp
        helpers/metrics_plotter_json.cpp
        helpers/gnb_console_helper.cpp
        helpers/metrics_hub.cpp
        adapters/e1ap_gateway_local_connector.cpp
        adapters/f1c_gateway_local_connector.cpp
        adapters/e2_gateway_remote_connector.cpp
        coverage_listener/zmq_cov.cpp # FUZZ
        )

install(TARGETS gnb
        RUNTIME)

target_link_libraries(gnb
        srsgnb_app_f1u_connector
        srsran_cu_up
        srsran_cu_cp
        srsran_network
        srsran_ngap
        ngap_asn1
        srsran_e2
        e2ap_asn1
        srsran_du_high
        srsran_du
        srsran_gateway
        mac_fapi_adaptor_factory
        phy_fapi_adaptor_factory
        srsran_fapi
        srsran_phy_support
        srsran_lower_phy
        srsran_upper_phy
        srsran_signal_processors
        srsran_channel_processors
        srsran_channel_equalizer
        srsran_channel_precoder
        srsran_ru_generic
        srsran_ru_ofh
        srsran_pcap
        srsran_support
        srsran_version
        srsran_build_info
        instr # FUZZ
        zmq_server_lib # FUZZ
        zmq # FUZZ
        shared_memory # FUZZ
        )

if (DPDK_FOUND)
    add_definitions(-DDPDK_FOUND)
    target_link_libraries(gnb hal_dpdk)
endif (DPDK_FOUND)

add_backward(gnb)

target_include_directories(gnb PRIVATE ${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/external)
