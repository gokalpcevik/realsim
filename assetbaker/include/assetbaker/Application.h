#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <boost/program_options.hpp>
#include <assimp/Importer.hpp>

#include "Logger.h"
#include "MeshBaker.h"
#include "Defaults.h"

#define BAKER_EXIT_SUCCESS EXIT_SUCCESS
#define BAKER_EXIT_FAILURE EXIT_FAILURE

#define RSIM_ASSET_BAKER_VER_MAJOR 0
#define RSIM_ASSET_BAKER_VER_MINOR 1
#define RSIM_ASSET_BAKER_VER_PATCH 0

namespace RSim::AssetBaker
{
	namespace po = boost::program_options;

	struct CommandLineArguments
	{
		CommandLineArguments(CommandLineArguments const&) = default;
		CommandLineArguments(CommandLineArguments&&) = default;

		int argc;
		char** argv;
	};

	class Application
	{
	public:
		Application(CommandLineArguments args);

		[[nodiscard]] int32_t Run();

	private:
		po::options_description GetGenericOptions();
		po::options_description GetConfigOptions() const;
		po::options_description GetHiddenOptions() const;
		[[nodiscard]] std::string GetVersionString() const;

		int32_t CreateConfigFile();
		int32_t PrintHelp();
		int32_t PrintVersion() const;
		bool CheckConfigFile(po::variables_map& vm, po::options_description const& cfgOptions) const;
		static MeshImportOptions GetMeshImportOptions(po::variables_map const& vm);
	private:
		CommandLineArguments m_Args;
		MeshImportOptions m_MeshImpOpt{};
		std::string m_ConfigFile{ DefaultConfigFile };
	};

}
