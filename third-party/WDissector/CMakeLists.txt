cmake_minimum_required(VERSION 3.7)

project(wdissector)

set(WS_SOURCE_DIR libs/wireshark)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
	${WS_SOURCE_DIR}
	${WS_SOURCE_DIR}/build/
	${WS_SOURCE_DIR}/include/
    libs/libpcap/
    libs/
  /usr/include/dbus-1.0
	/usr/include/glib-2.0
	/usr/lib/x86_64-linux-gnu/glib-2.0/include
)

link_directories(
    libs/wireshark/build/run
    /usr/lib/x86_64-linux-gnu
)

add_library(
  wireapi STATIC
  libs/whereami.c
  libs/profiling.c
  libs/logs.c
  ${WS_SOURCE_DIR}/ui/cmdarg_err.c
  ${WS_SOURCE_DIR}/ui/clopts_common.c
  ${WS_SOURCE_DIR}/ui/util.c
  ${WS_SOURCE_DIR}/ui/dissect_opts.c
  ${WS_SOURCE_DIR}/ui/failure_message.c
  ${WS_SOURCE_DIR}/ui/iface_toolbar.c
  ${WS_SOURCE_DIR}/ui/decode_as_utils.c
  ${WS_SOURCE_DIR}/ui/version_info.c
  ${WS_SOURCE_DIR}/frame_tvbuff.c
  ${WS_SOURCE_DIR}/capture/capture-pcap-util.c
  ${WS_SOURCE_DIR}/capture/capture-pcap-util-unix.c
  ${WS_SOURCE_DIR}/cfile.c
  ${WS_SOURCE_DIR}/file.c
  ${WS_SOURCE_DIR}/file_packet_provider.c
  ${WS_SOURCE_DIR}/extcap.c
  ${WS_SOURCE_DIR}/extcap_parser.c
  ${WS_SOURCE_DIR}/wsutil/please_report_bug.c)
target_compile_options(wireapi PRIVATE ${GLIB_CFLAGS} -fPIC -w)
target_include_directories(wireapi PUBLIC ${WDISSECTOR_INCLUDES})
target_link_libraries(
  wireapi
  PUBLIC ${GLIB_LDFLAGS}
         wireshark
         wiretap
         wsutil
         ${CMAKE_CURRENT_SOURCE_DIR}/libs/libpcap/libpcap.a
         dbus-1
         ibverbs
         -lgmodule-2.0
         -lz
         -lm
         -lpcre2-8)

add_library(wdissector SHARED wdissector.cpp)
target_link_libraries(wdissector PUBLIC wireapi writecap capchild caputils ui version_info glib-2.0)
target_compile_options(wdissector PRIVATE "-DWDISSECTOR_STATIC_LIB")
set_target_properties(wdissector PROPERTIES CXX_VISIBILITY_PRESET default
                                            VISIBILITY_INLINES_HIDDEN 0)