#include "realsim/core/Logger.h"

namespace RSim::Core
{
	std::shared_ptr<spdlog::async_logger> Logger::s_Logger;
	std::shared_ptr<spdlog::async_logger> Logger::s_ClientLogger;

	void Logger::Init()
	{
		spdlog::init_thread_pool(8192, 2);

		std::vector<spdlog::sink_ptr> logSinks;
		auto stdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		logSinks.emplace_back(stdoutSink);
		
		logSinks[0]->set_pattern("%T [%n] %^%v%$ ");

		s_Logger = std::make_shared<spdlog::async_logger>("LIB", std::begin(logSinks), std::end(logSinks), spdlog::thread_pool());
		spdlog::register_logger(s_Logger);
		s_Logger->set_level(spdlog::level::trace);
		s_Logger->flush_on(spdlog::level::trace);

		s_ClientLogger = std::make_shared<spdlog::async_logger>("APP",stdoutSink, spdlog::thread_pool());
		spdlog::register_logger(s_ClientLogger);
		s_ClientLogger->set_level(spdlog::level::trace);
		s_ClientLogger->flush_on(spdlog::level::trace);
	}
}