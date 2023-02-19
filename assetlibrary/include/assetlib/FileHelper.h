#pragma once
#include "MinWindows.h"
#include <cstdint>
#include <string>

namespace RSim::AssetLib
{
	[[nodiscard]] int64_t GetFileSize(std::wstring const& FilePath);
}
