#pragma once

#include <CoreAsset/AssetPtr.h>
#include <CoreAsset/IntermediateAsset.h>
#include <CoreBase/CallbackSystem.h>

namespace CoreAsset
{
class AssetManager;
struct IntermediateAsset;
class AssetMetaDataManager;

} // namespace CoreAsset

namespace QuadLF
{

class LogicalFileSystem;
class LogicalFolder;
} // namespace QuadLF

class Map;

using OnMapAssetAdded = Core::MultiCallbackSystem<Map *>;
using OnMapAssetRemoving = Core::MultiCallbackSystem<Map *>;

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
                                    CoreAsset::IntermediateAsset *intermediateAssetData,
                                    const CoreAsset::AssetCreationContext &creationContext = {});

    Map *CreateNewMap();

    // 대상 폴더에 복제하고 파일 생성 알림까지 연결한다. 폴더 생략 시 파일시스템의 현재 폴더를 사용한다.
    // domain은 대상 폴더로 결정하며, 실패 시 nullptr을 반환한다.
    CoreAsset::AssetPtr DuplicateAsset(CoreAsset::AssetPtr source, QuadLF::LogicalFolder *destFolder = nullptr);

    template <typename T> T *GetAsset(CoreAsset::AssetID id);

    OnMapAssetAdded mOnMapAssetAddedCallbackSystem;
    OnMapAssetRemoving mOnMapAssetRemovingCallbackSystem;

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
