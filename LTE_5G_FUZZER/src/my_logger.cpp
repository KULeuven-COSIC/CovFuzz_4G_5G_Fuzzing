#include "my_logger.h"

#include "helpers.h"

#include <filesystem>
#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

My_Logger my_logger_g;

bool My_Logger::init(const std::filesystem::path& log_dir_path) {
    std::filesystem::path debug_log_dir_path = log_dir_path / std::filesystem::path("debug");
    std::filesystem::path info_log_dir_path  = log_dir_path / std::filesystem::path("info");

    if (!helpers::create_directories_if_not_exist(debug_log_dir_path)) {
        std::cerr << "Could not create a directory: " << debug_log_dir_path << std::endl;
        return false;
    }
    if (!helpers::create_directories_if_not_exist(info_log_dir_path)) {
        std::cerr << "Could not create a directory: " << info_log_dir_path << std::endl;
        return false;
    }

    std::string timestamp = helpers::get_current_time_stamp();
    file_debug_path_ = debug_log_dir_path /  std::filesystem::path("srslog_debug_" + timestamp + ".log");
    file_info_path_ = info_log_dir_path / std::filesystem::path("srslog_info_" + timestamp + ".log");
    
    file_debug_sink_ = std::make_shared<spdlog::sinks::basic_file_sink_mt>(file_debug_path_.string());
    file_debug_sink_->set_level(spdlog::level::debug);

    file_info_sink_  = std::make_shared<spdlog::sinks::basic_file_sink_mt>(file_info_path_.string());
    file_info_sink_->set_level(spdlog::level::info);

    sinks_.push_back(file_debug_sink_);
    sinks_.push_back(file_info_sink_);

    logger = std::make_shared<spdlog::logger>("logger", sinks_.begin(), sinks_.end());

    spdlog::register_logger(logger);

    spdlog::set_level(spdlog::level::debug);

    spdlog::flush_on(spdlog::level::err);

    return true;
}