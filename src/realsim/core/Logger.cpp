#include "realsim/core/Logger.h"

namespace rsim::core
{
	std::shared_ptr<spdlog::async_logger> Logger::s_Logger;

	void Logger::Init()
	{
		spdlog::init_thread_pool(8192, 2);

		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("RealSim.log", true));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		s_Logger = std::make_shared<spdlog::async_logger>("RealSim", std::begin(logSinks), std::end(logSinks), spdlog::thread_pool());
		spdlog::register_logger(s_Logger);
		s_Logger->set_level(spdlog::level::trace);
		s_Logger->flush_on(spdlog::level::trace);
	}
}