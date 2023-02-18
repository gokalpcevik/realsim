#include "assetbaker/Application.h"
#include <fstream>

namespace RSim::AssetBaker
{
	Application::Application(CommandLineArguments args) : m_Args(args)
	{

	}

	int32_t Application::Run()
	{
        AssetBaker::Logger::Init();

        auto const generic = GetGenericOptions();
        auto const config  = GetConfigOptions();
		auto const hidden  = GetHiddenOptions();

        po::options_description cmdLineOptions;
        cmdLineOptions.add(generic).add(config).add(hidden);

        po::options_description cfgFileOptions;
        cfgFileOptions.add(config).add(hidden);

        po::options_description visibleOptions("Allowed Options");
        visibleOptions.add(generic).add(config);

        po::positional_options_description positionalOptions;
        positionalOptions.add("input-file", -1);

        try
        {
            po::variables_map vm;
            po::store(po::command_line_parser(m_Args.argc, m_Args.argv).options(cmdLineOptions).positional(positionalOptions).run(), vm);
            po::notify(vm);

            if(vm.count("create-config"))
            {
                return CreateConfigFile();
            }

            if (vm.count("help"))
            {
                return PrintHelp();
            }

            if (vm.count("version"))
            {
                return PrintVersion();
            }

			if(!CheckConfigFile(vm,cfgFileOptions)) return BAKER_EXIT_FAILURE;

            if (vm.count("input-file"))
            {
                auto&& outputDirectory = vm["output-directory"].as<std::string>();
                auto&& inputFiles = vm["input-file"].as<std::vector<std::string>>();

                // TODO:This part can be probably be multi-threaded
            	for (auto  const& file : inputFiles)
                {
                    if (std::filesystem::exists(file))
                    {
                        MeshImportOptions opt = GetMeshImportOptions(vm);
                        MeshBaker meshBaker(file, opt);
                    }
                    else
                    {
                        baker_error("The specified file '{0}' does not exist.", file);
                        continue;
                    }
                }
            }
        }
        catch(std::exception const& e)
        {
            baker_error(e.what());
            return BAKER_EXIT_FAILURE;
        }

		return BAKER_EXIT_SUCCESS;
	}

	po::options_description Application::GetGenericOptions()
	{
        po::options_description generic("Generic options");
        generic.add_options()
            ("version,v", "print the current asset baker version")
            ("help", "list of available options")
			("config,c", po::value<std::string>(&m_ConfigFile)->default_value(DefaultConfigFile.data(), DefaultConfigFile.data()), "configuration file")
            ("recursive", po::value<bool>()->default_value(false,"false"), "recurse through directories")
            ("merge-meshes,m", po::value<bool>(&m_MeshImpOpt.MergeIntoSingleFile)->default_value(false,"false"), "merge scene meshes into a single mesh asset")
			("create-config", po::value<std::string>(&m_ConfigFile), "create an empty config file");
        return generic;
	}

	po::options_description Application::GetConfigOptions() const
	{
        po::options_description config("Configuration");
        config.add_options()
            ("include-path,I", po::value< std::vector<std::string> >()->composing(), "include path")
			("output-directory,o", po::value< std::string >()->composing()->default_value(DefaultOutputDir.data()), "output directory");
        return config;
	}

	po::options_description Application::GetHiddenOptions() const
	{
        po::options_description hidden("Hidden options");
        hidden.add_options()
            ("input-file,f", po::value< std::vector<std::string> >()->composing(), "input file");
        return hidden;
	}

	std::string Application::GetVersionString() const
	{
        return fmt::format("{0}.{1}.{2}",
            RSIM_ASSET_BAKER_VER_MAJOR,
            RSIM_ASSET_BAKER_VER_MINOR,
            RSIM_ASSET_BAKER_VER_PATCH);
	}

	int32_t Application::CreateConfigFile()
	{
        std::ofstream of(m_ConfigFile);
        std::filesystem::path fsFile(m_ConfigFile);
        if(fsFile.extension() != ".cfg")
        {
            baker_error("Config files needs to have the extension '.cfg'.");
            return BAKER_EXIT_FAILURE;
        }

        if (of)
        {
            baker_info("Created the config file '{0}'", m_ConfigFile);
            return BAKER_EXIT_SUCCESS;
        }
        else
        {
            baker_error("Cannot create the config file '{0}'", m_ConfigFile);
            return BAKER_EXIT_FAILURE;
        }
	}

	int32_t Application::PrintHelp()
	{
        std::cout << GetGenericOptions() << std::endl;
        return BAKER_EXIT_SUCCESS;
	}

	int32_t Application::PrintVersion() const
	{
        baker_info("RealSimAssetBaker Version {0}", GetVersionString());
        return BAKER_EXIT_SUCCESS;
	}

	bool Application::CheckConfigFile(po::variables_map& vm, po::options_description const& cfgOptions) const
	{
        std::ifstream ifs(m_ConfigFile.c_str());

        if (!ifs)
        {
            baker_error("Config file '{}' is missing.", m_ConfigFile);
            return false;
        }
        else
        {
            store(po::parse_config_file(ifs, cfgOptions), vm);
            notify(vm);
            return true;
        }
	}

	MeshImportOptions Application::GetMeshImportOptions(po::variables_map const& vm)
	{
        MeshImportOptions options;
        options.MergeIntoSingleFile = vm["merge-meshes"].as<bool>();
        return options;
	}
}
