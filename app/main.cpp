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

using namespace rsim::core;

int main(int argc, char** argv)
{
    auto* application = new Application({argc, argv});

	WindowDescriptor desc{ "RealSim", 1600, 900, SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED ,0};
	auto window = std::make_unique<Window>(desc);

	application->SetMainWindow(std::move(window));
	return application->Run();
}
