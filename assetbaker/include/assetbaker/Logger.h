#pragma once
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/wincolor_sink.h>

namespace RSim::AssetBaker
{
    class Logger
    {
    private:
        static std::shared_ptr<spdlog::async_logger> s_Logger;

    public:
        Logger() = default;
        static void Init();
        static auto GetLogger() -> std::shared_ptr<spdlog::async_logger>& { return s_Logger; }
    };
}

#define baker_trace(...)    ::RSim::AssetBaker::Logger::GetLogger()->trace(__VA_ARGS__)
#define baker_debug(...)    ::RSim::AssetBaker::Logger::GetLogger()->debug(__VA_ARGS__)
#define baker_info(...)     ::RSim::AssetBaker::Logger::GetLogger()->info(__VA_ARGS__)
#define baker_warn(...)     ::RSim::AssetBaker::Logger::GetLogger()->warn(__VA_ARGS__)
#define baker_error(...)    ::RSim::AssetBaker::Logger::GetLogger()->error(__VA_ARGS__)
#define baker_critical(...) ::RSim::AssetBaker::Logger::GetLogger()->critical(__VA_ARGS__)
