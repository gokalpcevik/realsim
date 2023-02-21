# This file specifies how the project should be built, using CMake.
# If you are unfamiliar with CMake, don't worry about all the details.
# The sections you might want to edit are marked as such, and
# the comments should hopefully make most of it clear.
#
# For many purposes, you may not need to change anything about this file.

cmake_minimum_required(VERSION 3.21)

# Set project name, version and laguages here. (change as needed)
# Version numbers are available by including "exampleConfig.h" in
# the source. See exampleConfig.h.in for some more details.
project(realsim VERSION 0.1.0 LANGUAGES CXX)


# Options: Things you can set via commandline options to cmake (e.g. -DENABLE_LTO=[ON|OFF])
option(ENABLE_WARNINGS_SETTINGS "Allow target_set_warnings to add flags and defines.
                                 Set this to OFF if you want to provide your own warning parameters." ON)
option(ENABLE_LTO "Enable link time optimization" ON)
option(ENABLE_DOCTESTS "Include tests in the library. Setting this to OFF will remove all doctest related code.
                        Tests in tests/*.cpp will still be enabled." ON)

# Include stuff. No change needed.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(ConfigSafeGuards)
include(Colors)
include(CTest)
include(Doctest)
include(Documentation)
include(LTO)
include(Misc)
include(Warnings)
include(ExternalProject)
# Check for LTO support.
find_lto(CXX)


# Find the necessary packages
find_package(SDL2 2.0.14 REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(directxtk12 CONFIG REQUIRED)
find_package(directxmath CONFIG REQUIRED)
find_package(d3d12-memory-allocator CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(lz4 CONFIG REQUIRED)
find_path(D3DX12_INCLUDE_DIRS "d3dx12.h")

set(TESTFILES        # All .cpp files in tests/
    tests/main.cpp
)
set(RSIM_LIB realsim_lib)  # Default name for the library built from src/*.cpp (change if you wish)
set(RSIM_ASSET_LIB realsim_asset_library)
set(RSIM_ASSET_BAKER realsim_asset_baker)
# --------------------------------------------------------------------------------
#                         Locate files (change as needed).
# --------------------------------------------------------------------------------
set(SOURCES          # All .cpp files in src/
    src/realsim/core/Application.cpp
    src/realsim/core/Window.cpp
    src/realsim/core/Logger.cpp
    src/realsim/core/Input.cpp

    src/realsim/graphics/RealSimGraphics.cpp
    src/realsim/graphics/GraphicsDevice.cpp
    src/realsim/graphics/GraphicsResource.cpp
    src/realsim/graphics/SwapChain.cpp
    src/realsim/graphics/Renderer.cpp
    src/realsim/graphics/RendererConfiguration.cpp
    src/realsim/graphics/MemoryAllocator.cpp
    src/realsim/graphics/MemoryAllocation.cpp
    src/realsim/graphics/CommandAllocator.cpp
    src/realsim/graphics/FreeListAllocator.cpp
    src/realsim/graphics/UploadBuffer.cpp
    src/realsim/graphics/CommandQueue.cpp
    src/realsim/graphics/ShaderCompiler.cpp
    src/realsim/graphics/Shader.cpp
    src/realsim/graphics/RootSignature.cpp
    src/realsim/graphics/ShaderReflection.cpp
    src/realsim/graphics/RootSignatureFileDeserializer.cpp
    src/realsim/graphics/Exception.cpp
    src/realsim/graphics/Helpers.cpp
    src/realsim/math/Alignment.cpp
    src/realsim/ecs/Entity.cpp
    src/realsim/ecs/Scene.cpp
    src/realsim/ecs/CommonComponents.cpp
    src/realsim/ecs/Link.cpp
    src/realsim/serialization/YAML.cpp
    src/realsim/utils/StringUtils.cpp
)

add_library(${RSIM_LIB} OBJECT ${SOURCES})
target_include_directories(${RSIM_LIB} PUBLIC ${PROJECT_SOURCE_DIR}/include)

# There's also (probably) doctests within the library, so we need to see this as well.
target_link_libraries(${RSIM_LIB} PUBLIC
        doctest
        d3d12
        d3dcompiler
        dxgi
        dxguid
        Microsoft::DirectXMath
        Microsoft::DirectXTK12
        spdlog::spdlog
        SDL2::SDL2
        Eigen3::Eigen
        fmt::fmt
        unofficial::d3d12-memory-allocator
        yaml-cpp
        EnTT::EnTT
        lz4::lz4
        ${RSIM_ASSET_LIB}
        )

target_include_directories(${RSIM_LIB} PUBLIC ${D3D12X_INCLUDE_DIRS})


# Set the compile options you want (change as needed).
#target_set_warnings(${RSIM_LIB_NAME} ENABLE ALL AS_ERROR ALL DISABLE Annoying)
# target_compile_options(${RSIM_LIB_NAME} ... )  # For setting manually.

# Add an executable for the file app/main.cpp.
# If you add more executables, copy these lines accordingly.
add_executable(realsim_interactive  app/Main.cpp)   # Name of exec. and location of file.
target_link_libraries(realsim_interactive PRIVATE  ${RSIM_LIB})  # Link the executable to library (if it uses it).
target_set_warnings(realsim_interactive DISABLE Annoying) # Set warnings (if needed).
target_enable_lto(realsim_interactive optimized)  # enable link-time-optimization if available for non-debug configurations

# Set the properties you require, e.g. what C++ standard to use. Here applied to library and main (change as needed).
set_target_properties(
    ${RSIM_LIB} realsim_interactive
      PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

add_subdirectory(assetlibrary)
add_subdirectory(assetbaker)
add_subdirectory(tests)
