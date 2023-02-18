find_package(lz4 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

set(RSIM_ASSET_LIB realsim_asset_library PARENT_SCOPE)

add_library(${RSIM_ASSET_LIB}
	src/AssetLoader.cpp
	src/MeshLoader.cpp)

target_compile_definitions(${RSIM_ASSET_LIB} PUBLIC ${RSIM_ASSET_LIB})
target_include_directories(${RSIM_ASSET_LIB} PUBLIC ${PROJECT_SOURCE_DIR}/assetlibrary/include)
target_link_libraries(${RSIM_ASSET_LIB} PUBLIC
    lz4::lz4
    nlohmann_json::nlohmann_json)

set_target_properties(${RSIM_ASSET_LIB}
      PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)