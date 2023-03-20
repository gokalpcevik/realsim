// Executables must have the following defined if the library contains
// doctest definitions. For builds with this disabled, e.g. code shipped to
// users, this can be left out.
#ifdef ENABLE_DOCTEST_IN_LIBRARY
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"
#endif

#define SDL_MAIN_HANDLED

#include <iostream>
#include "realsim/core/App.h"
#include "realsim/core/Window.h"

#include "assetlib/AssetLoader.h"
#include "assetlib/MeshLoader.h"

#include <thread>

using namespace RSim;

int main(int argc, char** argv)
{
	Core::WindowDescription desc{ "RealSim", 800, 600, SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SDL_WINDOW_RESIZABLE|SDL_WINDOW_MAXIMIZED};
	auto window = std::make_unique<Core::Window>(desc);
	Core::App::Get().SetMainWindow(std::move(window));
	return Core::App::Get().Run();
}
