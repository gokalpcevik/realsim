#include "assetbaker/ScopedTimer.h"
#include "assetbaker/Logger.h"

namespace RSim::AssetBaker
{
	ScopedTimer::ScopedTimer(std::string ScopeName) : m_ScopeName(std::move(ScopeName))
	{
		m_Start = std::chrono::steady_clock::now();
	}

	ScopedTimer::~ScopedTimer()
	{
		auto Elapsed = std::chrono::steady_clock::now() - m_Start;
		baker_info("{0} - Elapsed(us):{1}", m_ScopeName, std::chrono::duration_cast<std::chrono::microseconds>(Elapsed).count());
	}
}
