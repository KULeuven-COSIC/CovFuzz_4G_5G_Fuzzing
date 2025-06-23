# CovFuzz_EuroSP_2025

## Installation
### Source-based installation:
Clone this repository and run the following commands to build and install the full framework:
```bash
cd CovFuzz_EuroSP_2025
sudo ./build_script.sh -b -i -t all
```
If you only want to fuzz 4G, then installing [srsRAN Project](https://github.com/srsran/srsRAN_Project) and [Open5Gs](https://github.com/open5gs/open5gs) is not necessary. Similar remark about fuzzing only 5G, in this case installing [srsRAN 4G](https://github.com/srsran/srsRAN_4G) is not necessary. If you encounter any diffictules with building the corresponding projects, please refer to their official github pages.

Before starting the fuzzer, set the correct paths in the configs/Fuzzing_Settings/srsran_config.json file.

The fuzzer was tested on Ubuntu 22.04.

### Docker-based installation
Install docker, clone this repository and run the following commands:
```bash
cd CovFuzz_EuroSP_2025
sudo docker build . -t [NAME]
sudo docker run --rm -e DISPLAY=$DISPLAY --net=host --privileged -v /tmp/.X11-unix:/tmp/.X11-unix:ro -v /dev:/dev -it -t [NAME] /bin/bash
```
This will give you bash shell in the docker container. To run the fuzzer follow the steps from **Running the fuzzer** section below.

**NB!** Currently the srsUE in the docker container is not able to connect to srsRAN network. Therefore, only COTS UE fuzzing can be tested using the docker.

## Running the fuzzer
First run a fuzzer in the no-fuzzing settings to ensure that everything is working as expected.
```bash
cd LTE_5G_FUZZER
sudo ./build/LTE_5G_Fuzzer configs/Fuzzing_Strategies/none_config.json
```

## Config files
The fuzzer has numerous of config files, which are responsible for fuzzing settings.
### LTE_5G_FUZZER/Fuzzing_Settings/main_config.json
In this config file you can choose some of the basic fuzzing settings. For example, protocol to fuzz (4G, 5G), the target to fuzz, the network name, logging policy as well as specify the paths to all the other config files.
### LTE_5G_FUZZER/Fuzzing_Settings/serial_config.json
In this config file you can set serial interface parameters of the modem, i.e. baudrate and port(s).
### LTE_5G_FUZZER/Fuzzing_Settings/srsran_config.json
In this config file you can set the paths to srsRAN and Open5Gs directories as well as choose if you want to use the monitor (see what happens with the base station and the core network) during the fuzzing. This option can be useful for the debug purposes.
### LTE_5G_FUZZER/Fuzzing_Settings/timers_config.json
In this config file you can set the time-limits for standard UE operations, such as attach attempt, airplane mode off/on. Some UEs can take longer to attempt to connect to the network than others. If this is the case, consider increasing the AT_CONNECT_TO_NETWORK time.
### common/shm/config.json
In this config file you can choose which interception point will be used: layer (RRC or MAC) and direction (Uplink or Downlink).

### Fuzzing_Strategies/*
The config files in the Fuzzing_Strategies define strategy of the fuzzing, i.e. the length of the fuzzing, the algorithm used to guide the fuzzing, etc. By default these config files are provided:
- none_config.json: "No fuzzing" config file that one can use to try to successfully connect the UE to the network, ensuring that everything work as expected.
- none_config_with_predefined_mutation_seed.json and none_config_with_predefined_injection_seed.json: config files that illustrate how to use predefined seeds (case of mutation and replay (injection)). This strategy can be helpful when trying to replay the seed (reproduce a crash).
- random_config_with_coverage_feedback.json: Random fuzzing config file aimed to use the coverage feedback during the fuzzing.
- random_config_wo_coverage_feedback.json: Random fuzzing config file aimed not to use the coverage feedback during the fuzzing.

## Helper scripts
Here the helper python3 scripts are described. You can find them in the helper_scripts folder.
- helper_scripts/hex_arr_to_dec.py: simple script helping to convert hex string (from the fuzzer logs) to the decimal string. Can be used when creating replay injection seeds.
- helper_scripts/coverage_dir_visualizer.py: script visualizing the coverage files using iteration number in X-axis. Run it without command line arguments to see the usage information.
- helper_scripts/coverage_dir_visualizer_by_time.py: script visualizing the coverage files using time in X-axis. Run it without command line arguments to see the usage information.

## Coverage&Mutation visualization scripts
To reproduce the figures from our paper, please refer to the scripts in LTE_5G_FUZZER/visualize_scripts/visualize_coverage_scripts/ and LTE_5G_FUZZER/visualize_scripts/visualize_mutation_stats_scripts/ directories. For example:
```bash
cd LTE_5G_FUZZER
./visualize_scripts/visualize_coverage_scripts/<THE SCRIPT YOU WANT TO RUN>
```
