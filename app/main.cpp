// Executables must have the following defined if the library contains
// doctest definitions. For builds with this disabled, e.g. code shipped to
// users, this can be left out.
#ifdef ENABLE_DOCTEST_IN_LIBRARY
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"
#endif

#include <iostream>
#include "realsim/core/application.h"

int main(int argc, char** argv)
{
    auto* app = new rsim::core::application({argc, argv});
    return app->Run();
}
