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
		std::chrono::time_point<std::chrono::steady_clock> m_Start;
		std::string m_ScopeName;
	};
}
