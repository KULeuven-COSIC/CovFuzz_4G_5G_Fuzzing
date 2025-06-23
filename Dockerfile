FROM ubuntu:22.04

WORKDIR /app/CovLTE_4G_5G

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt upgrade
# Install GIT
RUN apt install git -y

## For srsRAN_4G (https://docs.srsran.com/projects/4g/en/latest/general/source/1_installation.html)
RUN apt install -y build-essential cmake libfftw3-dev libmbedtls-dev libboost-program-options-dev libconfig++-dev libsctp-dev
# UHD USRP
RUN apt install -y libuhd-dev uhd-host
RUN uhd_images_downloader
# Clang
RUN apt install -y wget lsb-release software-properties-common gnupg
RUN wget https://apt.llvm.org/llvm.sh && chmod +x llvm.sh && ./llvm.sh 15

## For srsRAN_Project (https://docs.srsran.com/projects/project/en/latest/user_manuals/source/installation.html)
RUN apt-get install -y cmake make gcc g++ pkg-config libfftw3-dev libmbedtls-dev libsctp-dev libyaml-cpp-dev libgtest-dev

## For Open5Gs (https://open5gs.org/open5gs/docs/guide/02-building-open5gs-from-sources/)
RUN apt install -y python3-pip python3-setuptools python3-wheel ninja-build build-essential flex bison git cmake libsctp-dev \
        libgnutls28-dev libgcrypt-dev libssl-dev libidn11-dev libmongoc-dev libbson-dev libyaml-dev libnghttp2-dev \
        libmicrohttpd-dev libcurl4-gnutls-dev libnghttp2-dev libtins-dev libtalloc-dev meson
# Mongo DB
RUN apt install -y gnupg
RUN curl -fsSL https://pgp.mongodb.com/server-6.0.asc | gpg -o /usr/share/keyrings/mongodb-server-6.0.gpg --dearmor
RUN echo "deb [ arch=amd64,arm64 signed-by=/usr/share/keyrings/mongodb-server-6.0.gpg] https://repo.mongodb.org/apt/ubuntu jammy/mongodb-org/6.0 multiverse" | tee /etc/apt/sources.list.d/mongodb-org-6.0.list
RUN apt update
RUN apt install -y mongodb-org

## For WDissector
RUN apt install -y libglib2.0-dev libc-ares-dev qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools qttools5-dev qtmultimedia5-dev libspeexdsp-dev libcap-dev libibverbs-dev

## For Fuzzer
# LibSerial
RUN apt install -y g++ git autogen autoconf build-essential cmake graphviz libboost-dev libboost-test-dev libgtest-dev libtool \
             python3-sip-dev doxygen python3-sphinx pkg-config python3-sphinx-rtd-theme
# ZMQ
RUN apt-get install -y libzmq3-dev
# SPDLOG
RUN apt install libspdlog-dev
# Additional
RUN apt install -y dbus-x11 python3 udo psmisc nano procps libcanberra-gtk-module libcanberra-gtk3-module

# Install NLOHMANN JSON
RUN git clone https://github.com/nlohmann/json.git && cd json && mkdir -p build && cd build && cmake .. && make && make install

# Install spdlog
RUN git clone https://github.com/crayzeewulf/libserial.git && cd libserial && ./compile.sh && cd build && make install

# Install libzmq
RUN git clone https://github.com/zeromq/libzmq.git && cd libzmq && ./autogen.sh && ./configure && make && make install && ldconfig

# Install cppzmq
RUN git clone https://github.com/zeromq/cppzmq.git && cd cppzmq && mkdir -p build && cd build && cmake .. && make && make install


COPY third-party/ ./third-party/

# Build wireshark
RUN cd third-party/WDissector/libs/wireshark && rm -rf build && mkdir -p build && cd build && cmake .. && make

# Build WDissector
RUN cd third-party/WDissector/ && rm -rf build && mkdir -p build && cd build && cmake .. && make

## Build common dir
COPY common/Coverage_Instrumentation/ ./common/Coverage_Instrumentation/
COPY common/ZMQ/ ./common/ZMQ/
COPY common/shm/*.cpp ./common/shm/
COPY common/shm/*.h ./common/shm/
COPY common/shm/*.txt ./common/shm/
COPY common/*.txt ./common/
# Set the correct path to shm config file
ARG fuzz_config_file_path=/app/CovLTE_4G_5G/common/shm/config.json
RUN sed -i "s|FUZZ_CONFIG_PATH_PLACEHOLDER|$fuzz_config_file_path|g" common/shm/fuzz_config.h
# Build common dir
RUN cd common && rm -rf build && mkdir -p build && cd build && cmake .. && make

ENV THIRD_PARTY_PATH="/app/CovLTE_4G_5G/third-party/"
ENV COMMON_INCLUDE_PATH="/app/CovLTE_4G_5G/common/"
ENV COMMON_BUILD_PATH="/app/CovLTE_4G_5G/common/build/"

# Build Open5Gs
RUN cd third-party/open5gs && rm -rf build && meson build --prefix=`pwd`/install && ninja -C build && cd build && ninja install

# Set CMake compiler
ENV CC=/usr/bin/clang-15
ENV CXX=/usr/bin/clang++-15

# Build srsRAN_4G
RUN cd third-party/srsRAN_4G && rm -rf build && mkdir -p build && cd build && cmake .. && make

# Set srsRAN config files
RUN mkdir -p /etc/srsran
RUN cp third-party/srsRAN_4G/srsue/ue.conf.example /etc/srsran/ue.conf
RUN cp third-party/srsRAN_4G/srsepc/mbms.conf.example /etc/srsran/mbms.conf
RUN cp third-party/srsRAN_4G/srsepc/epc.conf.example /etc/srsran/epc.conf
RUN cp third-party/srsRAN_4G/srsenb/rb.conf.example /etc/srsran/rb.conf
RUN cp third-party/srsRAN_4G/srsenb/sib.conf.example /etc/srsran/sib.conf
RUN cp third-party/srsRAN_4G/srsenb/enb.conf.example /etc/srsran/enb.conf
RUN cp third-party/srsRAN_4G/srsenb/rr.conf.example /etc/srsran/rr.conf
RUN cp third-party/srsRAN_4G/srsenb/sib.conf.example /etc/srsran/sib.conf
RUN cp third-party/srsRAN_4G/srsepc/user_db.csv.example /etc/srsran/user_db.csv

# Build srsRAN_Project
RUN cd third-party/srsRAN_Project && rm -rf build && mkdir -p build && cd build && cmake .. && make

# Unset compiler-specifying variables
RUN unset CC
RUN unset CXX

# Build fuzzer
COPY LTE_5G_FUZZER/src/ ./LTE_5G_FUZZER/src/
COPY LTE_5G_FUZZER/include/ ./LTE_5G_FUZZER/include/
COPY LTE_5G_FUZZER/CMakeLists.txt ./LTE_5G_FUZZER/
RUN cd LTE_5G_FUZZER && rm -rf build && mkdir -p build && cd build && cmake .. && make

RUN unset THIRD_PARTY_PATH
RUN unset COMMON_INCLUDE_PATH
RUN unset COMMON_BUILD_PATH

COPY common/ ./common/
COPY LTE_5G_FUZZER/ ./LTE_5G_FUZZER/
COPY generate_srsran_config.py ./

# Generate the srsran config file
RUN python3 generate_srsran_config.py

WORKDIR /app/CovLTE_4G_5G/LTE_5G_FUZZER

CMD [ "./build/LTE_5G_Fuzzer" ]