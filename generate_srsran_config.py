import json
import os

SRSRAN_CONFIG_PATH = "LTE_5G_FUZZER/configs/Fuzzing_Settings/srsran_config.json"
CUR_DIR = os.getcwd()

dictionary = {
    "path_to_srsran_4G": f"{CUR_DIR}/third-party/srsRAN_4G/build",
    "path_to_srsran_project": f"{CUR_DIR}/third-party/srsRAN_Project",
    "path_to_srsran_project_config": f"{CUR_DIR}/third-party/srsRAN_Project/configs/gnb_b210_20MHz_band_78_FUZZ.yml",
    "path_to_open5gs": f"{CUR_DIR}/third-party/open5gs",
    "close_base_station_after_fuzzing": False,
    "close_core_network_after_fuzzing": False,
    "close_ue_after_fuzzing": False,
    "use_monitor": True
}

json_object = json.dumps(dictionary, indent=4)

with open(SRSRAN_CONFIG_PATH, "w") as outfile:
    outfile.write(json_object)