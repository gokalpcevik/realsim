#include "assetbaker/Application.h"

int main(int argc, char** argv)
{
    char*  args[] = { "dummy", "--create-config", "asset_baker.cfg" };
    int32_t Return = RSim::AssetBaker::Application({argc,argv}).Run();
    // There is an issue with spdlog multithreaded loggers in MSVC that causes busy mutexes being destroyed before the
    // application shuts down. We need to call shutdown() before exiting.
    spdlog::shutdown();
    return Return;
}