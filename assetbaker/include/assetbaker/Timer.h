#pragma once
#include <chrono>
#include <iostream>

namespace RSim::AssetBaker
{
	class ScopedTimer
	{
	public:
		ScopedTimer(std::string ScopeName);
		~ScopedTimer();

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
		std::string m_ScopeName;
	};

	class ManualTimer
	{
	public:
		ManualTimer() = default;
		~ManualTimer() = default;

		void Start(std::string TimerName);
		void Stop();
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
		std::string m_TimerName{};
	};
}
