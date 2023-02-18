#include "assetbaker/Logger.h"

namespace RSim::AssetBaker
{
	std::shared_ptr<spdlog::async_logger> Logger::s_Logger;

	void Logger::Init()
	{
		spdlog::init_thread_pool(8192, 4);
		std::vector<spdlog::sink_ptr> logSinks;
		auto stdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		auto msvcSink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
		logSinks.emplace_back(stdoutSink);
		logSinks.emplace_back(msvcSink);
		
		logSinks[0]->set_pattern("%T [%l] %^%v%$ ");
		logSinks[1]->set_pattern("%T [%l] %^%v%$ ");

		s_Logger = std::make_shared<spdlog::async_logger>("ASSET_BAKER", std::begin(logSinks), std::end(logSinks), spdlog::thread_pool());
		spdlog::register_logger(s_Logger);
		s_Logger->set_level(spdlog::level::trace);
		s_Logger->flush_on(spdlog::level::trace);
	}
}