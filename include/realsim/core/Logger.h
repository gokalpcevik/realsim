#pragma once
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace rsim
{
    namespace core
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
}

#define rsim_trace(...)    ::rsim::core::Logger::GetLogger()->trace(__VA_ARGS__)
#define rsim_debug(...)    ::rsim::core::Logger::GetLogger()->debug(__VA_ARGS__)
#define rsim_info(...)     ::rsim::core::Logger::GetLogger()->info(__VA_ARGS__)
#define rsim_warn(...)     ::rsim::core::Logger::GetLogger()->warn(__VA_ARGS__)
#define rsim_error(...)    ::rsim::core::Logger::GetLogger()->error(__VA_ARGS__)
#define rsim_critical(...) ::rsim::core::Logger::GetLogger()->critical(__VA_ARGS__)