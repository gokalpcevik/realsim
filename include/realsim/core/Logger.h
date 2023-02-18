#pragma once
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/wincolor_sink.h>

namespace RSim
{
    namespace Core
    {
        class Logger
        {
        private:
            static std::shared_ptr<spdlog::async_logger> s_Logger;
            static std::shared_ptr<spdlog::async_logger> s_ClientLogger;

        public:
            Logger() = default;
            static void Init();
            static auto GetLogger() -> std::shared_ptr<spdlog::async_logger>& { return s_Logger; }
            static auto GetClientLogger() -> std::shared_ptr<spdlog::async_logger>& { return s_ClientLogger; }
        };
    }
}

#define rsim_trace(...)    ::RSim::Core::Logger::GetLogger()->trace(__VA_ARGS__)
#define rsim_debug(...)    ::RSim::Core::Logger::GetLogger()->debug(__VA_ARGS__)
#define rsim_info(...)     ::RSim::Core::Logger::GetLogger()->info(__VA_ARGS__)
#define rsim_warn(...)     ::RSim::Core::Logger::GetLogger()->warn(__VA_ARGS__)
#define rsim_error(...)    ::RSim::Core::Logger::GetLogger()->error(__VA_ARGS__)
#define rsim_critical(...) ::RSim::Core::Logger::GetLogger()->critical(__VA_ARGS__)

#define rsim_client_trace(...)    ::RSim::Core::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define rsim_client_debug(...)    ::RSim::Core::Logger::GetClientLogger()->debug(__VA_ARGS__)
#define rsim_client_info(...)     ::RSim::Core::Logger::GetClientLogger()->info(__VA_ARGS__)
#define rsim_client_warn(...)     ::RSim::Core::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define rsim_client_error(...)    ::RSim::Core::Logger::GetClientLogger()->error(__VA_ARGS__)
#define rsim_client_critical(...) ::RSim::Core::Logger::GetClientLogger()->critical(__VA_ARGS__)