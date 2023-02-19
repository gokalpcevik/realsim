#include "assetbaker/Application.h"
#include <fstream>

namespace RSim::AssetBaker
{
    using RecursiveDirectoryIt = std::filesystem::recursive_directory_iterator;
    using DirectoryIt = std::filesystem::directory_iterator;
    namespace fs = std::filesystem;

	Application::Application(CommandLineArguments args) : m_Args(args)
	{

	}

	int32_t Application::Run()
	{
        AssetBaker::Logger::Init();

        auto const generic = GetGenericOptions();
        auto const config  = GetConfigOptions();
		auto const hidden  = GetHiddenOptions();

        po::options_description CommandLineOptions;
        CommandLineOptions.add(generic).add(config).add(hidden);

        po::options_description CfgFileOptions;
        CfgFileOptions.add(config).add(hidden);

        po::options_description VisibleOptions("Allowed Options");
        VisibleOptions.add(generic).add(config);

        po::positional_options_description PositionalOptions;
        PositionalOptions.add("input-file", -1);

        try
        {
            po::variables_map vm;
            po::store(po::command_line_parser(m_Args.argc, m_Args.argv).options(CommandLineOptions).positional(PositionalOptions).run(), vm);
            po::notify(vm);

            if (vm.count("help"))
            {
                return PrintHelp();
            }

            if (vm.count("create-config"))
            {
                return CreateConfigFile();
            }

            if (vm.count("version"))
            {
                return PrintVersion();
            }

			if(!CheckConfigFile(vm,CfgFileOptions)) return BAKER_EXIT_FAILURE;

            auto&& OutputDirectory = vm["output-path"].as<std::string>();

            if (!fs::exists(OutputDirectory))
            {
                baker_error("The specified output path '{0}' does not exist.", OutputDirectory);
                return BAKER_EXIT_FAILURE;
            }

            if (fs::status(OutputDirectory).type() != fs::file_type::directory)
            {
                baker_error("The specified output path '{0} is not a directory.'", OutputDirectory);
                return BAKER_EXIT_FAILURE;
            }

            bool Recursive = vm["recursive"].as<bool>();

            if(vm.count("include-path"))
            {
                auto&& IncludePaths = vm["include-path"].as<std::vector<std::string>>();

                if (Recursive)
                {
                    for (std::string const& IncludePath : IncludePaths)
                    {
                        for (auto const& It : RecursiveDirectoryIt(IncludePath))
                        {
                            if(It.is_regular_file())
                            {
                                auto relative = fs::relative(It.path(), IncludePath);
                                auto out = OutputDirectory / relative;
                                fs::create_directories(out.parent_path());
                            	MeshImportOptions opt = GetMeshImportOptions(vm);
                                MeshBaker meshBaker(It, opt,out.parent_path());
                            }
                        }
                    }
                }
                else
                {
                    for (std::string const& IncludePath : IncludePaths)
                    {
                        for (auto const& It : DirectoryIt(IncludePath))
                        {
                            if(It.is_regular_file())
                            {
                                auto relative = fs::relative(It.path(), IncludePath);
                                auto out = OutputDirectory / relative;
                                fs::create_directories(out.parent_path());
                                MeshImportOptions opt = GetMeshImportOptions(vm);
                                MeshBaker meshBaker(It, opt,out.parent_path());
                            }
                        }
                    }
                }
            }
            else
            {
                baker_error("No input paths has been specified.");
                return BAKER_EXIT_FAILURE;
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
            ("recursive,r", po::value<bool>()->default_value(false,"false"), "recurse through directories")
            ("merge-meshes,m", po::value<bool>(&m_MeshImpOpt.MergeIntoSingleFile)->default_value(false,"false"), "merge scene meshes into a single mesh asset")
			("create-config", po::value<std::string>(&m_ConfigFile), "create an empty config file");
        return generic;
	}

	po::options_description Application::GetConfigOptions() const
	{
        po::options_description config("Configuration");
        config.add_options()
            ("include-path,I", po::value< std::vector<std::string> >()->composing(), "include path")
			("output-path,o", po::value< std::string >()->composing()->default_value(DefaultOutputDir.data()), "output path");
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
