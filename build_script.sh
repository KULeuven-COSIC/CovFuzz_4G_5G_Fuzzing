#!/bin/bash

############################ START: VARIABLE DEFINITIONS ######################################
WIRESHARK="wireshark"
WDISSECTOR="wdissector"
COMMON="common"
FUZZER="fuzzer"
SRSRAN_4G="srsran_4g"
SRSRAN_PROJECT="srsran_project"
OPEN_5GS="open5gs"
ALL="all"
declare -a targets=($WIRESHARK $WDISSECTOR $COMMON $FUZZER $SRSRAN_4G $SRSRAN_PROJECT $OPEN_5GS $ALL)
declare -a user_targets
# Build flag
build_flag_g=false
# Install flag
install_flag_g=false
############################ END: VARIABLE DEFINITIONS ######################################

############################ START: FUNCTION DEFINITIONS ######################################

function apt_install_all_packets {
    sudo apt update && sudo apt upgrade
    
    # GIT
    sudo apt install git -y

    ## For srsRAN_4G (https://docs.srsran.com/projects/4g/en/latest/general/source/1_installation.html)
    sudo apt install -y build-essential cmake libfftw3-dev libmbedtls-dev libboost-program-options-dev libconfig++-dev libsctp-dev
    # UHD USRP
    sudo apt install -y libuhd-dev uhd-host
    uhd_images_downloader
    # Clang
    wget https://apt.llvm.org/llvm.sh && chmod +x llvm.sh && sudo ./llvm.sh 15

    ## For srsRAN_Project (https://docs.srsran.com/projects/project/en/latest/user_manuals/source/installation.html)
    sudo apt-get install -y cmake make gcc g++ pkg-config libfftw3-dev libmbedtls-dev libsctp-dev libyaml-cpp-dev libgtest-dev

    ## For Open5Gs (https://open5gs.org/open5gs/docs/guide/02-building-open5gs-from-sources/)
    sudo apt install -y python3-pip python3-setuptools python3-wheel ninja-build build-essential flex bison git cmake libsctp-dev \
            libgnutls28-dev libgcrypt-dev libssl-dev libidn11-dev libmongoc-dev libbson-dev libyaml-dev libnghttp2-dev \
            libmicrohttpd-dev libcurl4-gnutls-dev libnghttp2-dev libtins-dev libtalloc-dev meson
    # Mongo DB
    sudo apt install -y gnupg
    curl -fsSL https://pgp.mongodb.com/server-6.0.asc | sudo gpg -o /usr/share/keyrings/mongodb-server-6.0.gpg --dearmor
    echo "deb [ arch=amd64,arm64 signed-by=/usr/share/keyrings/mongodb-server-6.0.gpg] https://repo.mongodb.org/apt/ubuntu jammy/mongodb-org/6.0 multiverse" | sudo tee /etc/apt/sources.list.d/mongodb-org-6.0.list
    sudo apt update
    sudo apt install -y mongodb-org
    sudo systemctl start mongod # if '/usr/bin/mongod' is not running
    sudo systemctl enable mongod # ensure to automatically start it on system boot

    ## For WDissector
    sudo apt install -y libglib2.0-dev libc-ares-dev qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools qttools5-dev qtmultimedia5-dev libspeexdsp-dev libcap-dev libibverbs-dev

    ## For Fuzzer
    # LibSerial
    sudo apt install -y g++ git autogen autoconf build-essential cmake graphviz libboost-dev libboost-test-dev libgtest-dev libtool \
                 python3-sip-dev doxygen python3-sphinx pkg-config python3-sphinx-rtd-theme android-tools-adb
    # ZMQ
    sudo apt-get install -y libzmq3-dev
    # SPDLOG
    sudo apt install libspdlog-dev
    # Additional
    sudo apt install -y dbus-x11 python3
}

function install_neccessary_libs() {
    # Apt install necessary packets
    apt_install_all_packets

    # Install everything in the third-party folder
    cd third-party
    start_dir_l=`pwd`

    # Install nlohmann json
    git clone https://github.com/nlohmann/json.git
    if ! build_install_dir json; then
        custom_exit $?
    fi
    cd $start_dir_l

    # Install libserial
    git clone https://github.com/crayzeewulf/libserial.git && cd libserial && ./compile.sh && cd build && sudo make install
    cd $start_dir_l

    # Install libzmq
    git clone https://github.com/zeromq/libzmq.git && cd libzmq
    ./autogen.sh
    ./configure
    make
    sudo make install && sudo ldconfig
    cd $start_dir_l

    # Install cppzmq`
    git clone https://github.com/zeromq/cppzmq.git
    if ! build_install_dir cppzmq; then
        custom_exit $?
    fi
    cd $start_dir_l
}

function build_success { echo "Build successful"; }
function build_fail { echo "Build failed"; }
function build_routine { mkdir -p build && cd build && cmake .. && cmake --build .; }

function build_dir { 
    success=0;
    start_dir=`pwd`;
    echo "Building $1";
    if cd $1 && build_routine; then
        build_success;
    else
        success=$?;
        build_fail;
    fi
    cd $start_dir;
    return $success
}

function build_install_dir { 
    start_dir_l=`pwd`
    if ! build_dir $1; then
        return $?
    fi
    start_dir_l=`pwd`
    cd $1 && cd build && sudo make install;
    success=$?
    cd $pwd
    return $success
}

function find_installed_clang_version {
    for i in {18..14}; do
        if which clang-$i > /dev/null; then
            echo $i;
            return 0;
        fi
    done
    echo "0"
    return;
}

function set_env_vars {
    export THIRD_PARTY_PATH="`pwd`/third-party/";
    export COMMON_INCLUDE_PATH="`pwd`/common/";
    export COMMON_BUILD_PATH="`pwd`/common/build/";
}

function unset_env_vars {
    unset THIRD_PARTY_PATH;
    unset COMMON_INCLUDE_PATH;
    unset COMMON_BUILD_PATH;
}

function custom_exit {
    unset_env_vars
    exit $1
}

function contains() {
    local value="$1"
    shift
    local array=("$@")
    for element in "${array[@]}"; do
        if [[ "$element" == "$value" ]]; then
            return 0
        fi
    done
    return 1
}

function usage() {
    echo "Usage: $0 [-h] [-b] [-i] [-t \"value1 value2 ...\"]"
    echo "  -h                : Show help"
    echo "  -b                : Build the targets"
    echo "  -i                : Install the targets"
    echo "  -t \"value1 value2 ...\" : Targets (Array of strings separated by spaces)"
    echo "Supported targets: \"${targets[@]}\""
    echo "Example (build and install all): $0 -b -i -t \"$ALL\""
    exit 1
}

function parse_flags() {
    while getopts ":biht:" opt; do
        case ${opt} in
            b)
                build_flag_g=true
                ;;
            i)
                install_flag_g=true
                ;;
            t)
                user_targets=(${OPTARG})
                ;;
            h)  
                usage
                ;;
            \?)
                echo "Invalid option: -${OPTARG}" >&2
                usage
                ;;
            :)
                echo "Option -${OPTARG} requires an argument." >&2
                usage
                ;;
        esac
    done
}

############################ END: FUNCTION DEFINITIONS ######################################

parse_flags "$@"

# echo "Supported targets: \"${targets[@]}\""
# user_targets=( "$@" )
# if [[ ${#user_targets[@]} -eq 0 ]]; then
#     echo "Please specify targets you want to build"
#     exit
# fi
echo "Build flag: $build_flag_g"
echo "Install flag: $install_flag_g"
echo "User chosen targets: ${user_targets[@]}"

# Set enviromental variables
set_env_vars

# Install necessary libraries
if $install_flag_g; then
    install_neccessary_libs
fi

# Remove randomization. That is due to the bug in ASAN, which causes crashes otherwise. 
# See https://stackoverflow.com/questions/78293129/c-programs-fail-with-asan-addresssanitizerdeadlysignal for details.
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space

if ($build_flag_g && (contains "$WIRESHARK" "${user_targets[@]}") || (contains "$ALL" "${user_targets[@]}")); then
    # Build WDissector
    ## Build special version of wireshark
    if ! build_dir third-party/WDissector/libs/wireshark; then
        custom_exit $?;
    fi
fi

if ($build_flag_g && (contains "$WDISSECTOR" "${user_targets[@]}") || (contains "$ALL" "${user_targets[@]}")); then
    ## Build wdissector
    if ! build_dir third-party/WDissector; then
        custom_exit $?;
    fi
fi

if ($build_flag_g && (contains "$COMMON" "${user_targets[@]}" || contains "$ALL" "${user_targets[@]}")); then
    # Build common directory
    fuzz_config_file_path=`pwd`/common/shm/config.json
    sed -i "s|FUZZ_CONFIG_PATH_PLACEHOLDER|$fuzz_config_file_path|g" common/shm/fuzz_config.h
    if ! build_dir common; then
        custom_exit $?;
    fi
fi

if (contains "$FUZZER" "${user_targets[@]}" || contains "$ALL" "${user_targets[@]}"); then
    if $install_flag_g; then
        python3 generate_srsran_config.py
    fi
    if $build_flag_g; then
        if ! build_dir LTE_5G_FUZZER; then
            custom_exit $?;
        fi
    fi
fi

# Build Open5Gs. It is using meson instead of cmake.
if (contains "$OPEN_5GS" "${user_targets[@]}" || contains "$ALL" "${user_targets[@]}"); then
    start_dir_l=`pwd`
    cd third-party/open5gs
    if ($build_flag_g); then
        meson build --prefix=`pwd`/install && ninja -C build
    fi
    if ($install_flag_g); then
        cd build && ninja install
    fi
    cd $start_dir_l
fi

# Build srsRAN_4G and srsRAN_Project. Use clang for that to enable the LLVM coverage instrumentation.
if ($build_flag_g && (contains "$SRSRAN_4G" "${user_targets[@]}" || contains "$SRSRAN_PROJECT" "${user_targets[@]}" || contains "$ALL" "${user_targets[@]}")); then
    supported_clang_version=$(find_installed_clang_version)
    echo "Sup: $supported_clang_version"
    if [ $supported_clang_version == "0" ]; then
        echo "Did not find installed clang";
        custom_exit;
    else
        echo "Found supported clang version: $supported_clang_version"
    fi
    export CC=`which clang-$supported_clang_version`
    export CXX=`which clang++-$supported_clang_version`
fi

if contains "$SRSRAN_4G" "${user_targets[@]}" || contains "$ALL" "${user_targets[@]}"; then
    if $build_flag_g; then
        if ! build_dir "third-party/srsRAN_4G"; then
            custom_exit $?
        fi
    fi
    if $install_flag_g; then
        sudo mkdir -p /etc/srsran
        sudo cp third-party/srsRAN_4G/srsue/ue.conf.example /etc/srsran/ue.conf
        sudo cp third-party/srsRAN_4G/srsepc/mbms.conf.example /etc/srsran/mbms.conf
        sudo cp third-party/srsRAN_4G/srsepc/epc.conf.example /etc/srsran/epc.conf
        sudo cp third-party/srsRAN_4G/srsenb/rb.conf.example /etc/srsran/rb.conf
        sudo cp third-party/srsRAN_4G/srsenb/sib.conf.example /etc/srsran/sib.conf
        sudo cp third-party/srsRAN_4G/srsenb/enb.conf.example /etc/srsran/enb.conf
        sudo cp third-party/srsRAN_4G/srsenb/rr.conf.example /etc/srsran/rr.conf
        sudo cp third-party/srsRAN_4G/srsenb/sib.conf.example /etc/srsran/sib.conf
        sudo cp third-party/srsRAN_4G/srsepc/user_db.csv.example /etc/srsran/user_db.csv
    fi
fi

if (contains "$SRSRAN_PROJECT" "${user_targets[@]}" || contains "$ALL" "${user_targets[@]}"); then
    if $build_flag_g; then
        if ! build_dir "third-party/srsRAN_Project"; then
            custom_exit $?
        fi
    fi
fi

unset_env_vars