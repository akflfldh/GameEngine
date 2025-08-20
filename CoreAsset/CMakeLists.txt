add_library(CoreAsset SHARED
Asset.h 
AssetType.h
AssetCommon.h 
AssetIDGenerator.h 
AssetIDTable.h
AssetMetaDataType.h 
AssetMetaDataManager.h 
AssetPtr.h 
CoreAssetDllMacro.h
GlobalAssetRegistrySystem.h 
Material.h 
SerializedAssetType.h 
Texture.h 
TextureManager.h 
UIMaterialManager.h 
AssetLoader.h
Texture.cpp
TextureLoader.h
TextureStorer.h
MaterialStorer.h
MaterialManager.h
MaterialLoader.h
baseAssetStorer.h


Asset.cpp 
AssetCommon.cpp 
AssetIDGenerator.cpp
AssetIDTable.cpp 
AssetMetaDataManager.cpp 
AssetPtr.cpp 
AssetMetaDataType.cpp
GlobalAssetRegistrySystem.cpp
Material.cpp 
SerializedAssetType.cpp
Texture.cpp
TextureManager.cpp 
UIMaterialManager.cpp 
Texture.cpp
TextureLoader.cpp
TextureManager.cpp
TextureStorer.cpp
MaterialStorer.cpp
MaterialManager.cpp
MaterialLoader.cpp
AssetLoader.cpp
BaseAssetStorer.cpp
)



target_compile_definitions(CoreAsset PRIVATE CORE_ASSET_EXPORT)

target_link_libraries(CoreAsset PUBLIC compiler_flags headerDir Logger GpuResourceManager Utility BinaryReaderWriter RenderSystem CoreDevice)

target_include_directories(CoreAsset PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})




