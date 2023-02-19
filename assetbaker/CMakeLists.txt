find_package(lz4 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(spdlog CONFIG REQUIRED)

set(RSIM_ASSET_BAKER PARENT_SCOPE)
set(RSIM_ASSET_LIB PARENT_SCOPE)

add_executable(${RSIM_ASSET_BAKER} 
    src/Main.cpp
    src/Timer.cpp
    src/MeshBaker.cpp
    src/Logger.cpp
    src/Application.cpp)

target_include_directories(${RSIM_ASSET_BAKER} PRIVATE ${PROJECT_SOURCE_DIR}/assetbaker/include)
target_include_directories(${RSIM_ASSET_BAKER} PRIVATE ${PROJECT_SOURCE_DIR}/assetlibrary/include)
target_include_directories(${RSIM_ASSET_BAKER} PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(${RSIM_ASSET_BAKER} PUBLIC
    lz4::lz4
    nlohmann_json::nlohmann_json
    assimp::assimp
    spdlog::spdlog
    Boost::program_options
    ${RSIM_ASSET_LIB})

set_target_properties(${RSIM_ASSET_BAKER}
      PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)