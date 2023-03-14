// Executables must have the following defined if the library contains
// doctest definitions. For builds with this disabled, e.g. code shipped to
// users, this can be left out.
#ifdef ENABLE_DOCTEST_IN_LIBRARY
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"
#endif

#define SDL_MAIN_HANDLED

#include <iostream>
#include "realsim/core/Application.h"
#include "realsim/core/Window.h"

#include "assetlib/AssetLoader.h"
#include "assetlib/MeshLoader.h"

#include <thread>

namespace Core = ::RSim::Core;
namespace AssetLib = ::RSim::AssetLib;

class RealSimInteractive : public Core::Application
{
public:
	RealSimInteractive(CommandLineArgs args);

	RSim::ReturnCode OnInit() override;
	void OnUpdate() override;

private:
	AssetLib::Asset MeshAsset;
	AssetLib::MeshInfo MeshInfo;
};

RealSimInteractive::RealSimInteractive(CommandLineArgs args) : Application(args)
{
}

RSim::ReturnCode RealSimInteractive::OnInit()
{
	return Application::OnInit();
}

void RealSimInteractive::OnUpdate()
{
	Application::OnUpdate();
}


int main(int argc, char** argv)
{
	auto* application = new RealSimInteractive({argc, argv});

	Core::WindowDescription desc{ "RealSim", 800, 600, SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SDL_WINDOW_RESIZABLE};
	auto window = std::make_unique<RSim::Core::Window>(desc);
	application->SetMainWindow(std::move(window));
	return application->Run();
}
