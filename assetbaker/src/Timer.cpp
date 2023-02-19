#include "assetbaker/Timer.h"
#include "assetbaker/Logger.h"

namespace RSim::AssetBaker
{
	ScopedTimer::ScopedTimer(std::string ScopeName) : m_ScopeName(std::move(ScopeName))
	{
		m_Start = std::chrono::high_resolution_clock::now();
	}

	ScopedTimer::~ScopedTimer()
	{
		auto Elapsed = std::chrono::high_resolution_clock::now() - m_Start;
		baker_info("{0} - Elapsed(us):{1}", m_ScopeName, std::chrono::duration_cast<std::chrono::microseconds>(Elapsed).count());
	}

	void ManualTimer::Start(std::string TimerName)
	{
		m_TimerName = std::move(TimerName);
		m_Start = std::chrono::high_resolution_clock::now();
	}

	void ManualTimer::Stop()
	{
		auto Elapsed = std::chrono::high_resolution_clock::now() - m_Start;
		baker_info("{0} - Elapsed: {1}us", m_TimerName, std::chrono::duration_cast<std::chrono::microseconds>(Elapsed).count());
	}
}
