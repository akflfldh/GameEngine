#pragma once

#include <CoreAsset/AssetPtr.h>

namespace CoreAsset
{
class AssetManager;
struct IntermediateAsset;
class AssetMetaDataManager;

} // namespace CoreAsset

namespace QuadLF
{

class LogicalFileSystem;
}

namespace Quad
{

// CoreAsset::AssetManager를 래핑하고, + 논리적 파일시스템 기능까지 수행 한다.
// ex 에셋 생성 -> 논리적파일 시스템에 등록
class EditorAssetManager
{
  public:
    static EditorAssetManager *GetInstance();
    EditorAssetManager();
    ~EditorAssetManager();

    void Initialize(CoreAsset::AssetManager *assetManager, QuadLF::LogicalFileSystem *logicalFileSystem,
                    CoreAsset::AssetMetaDataManager *assetMetaDataManager);

    CoreAsset::AssetPtr CreateAsset(CoreAsset::EAssetType assetType,
                                    CoreAsset::IntermediateAsset *intermediateAssetData);

    template <typename T> T *GetAsset(CoreAsset::AssetID id);

  private:
    // Get + 없다면 디폴트로 가져올것
    CoreAsset::AssetPtr GetAssetInner(const char *assetClassName, CoreAsset::AssetID id);

  private:
    CoreAsset::AssetManager *mAssetManager;
    QuadLF::LogicalFileSystem *mLogicalFileSystem;
    CoreAsset::AssetMetaDataManager *mAssetMetaDataManager;
};

template <typename T> T *EditorAssetManager::GetAsset(CoreAsset::AssetID id)
{
    static_assert(std::is_base_of_v<CoreAsset::Asset, T>,
                  "EditorAssetManager :: GetAsset; Type must derive from Asset ");

    return static_cast<T *>(GetAssetInner(CoreAsset::AssetName<T>::Get(), id).Get());
}

} // namespace Quad