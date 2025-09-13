#pragma once

#include <CoreAsset/AssetPtr.h>
#include <CoreAsset/AssetType.h>
#include <CoreAsset/CoreAssetDLLMacro.h>
#include <CoreBase/FVector.h>
namespace CoreAsset
{

class Asset;
class AssetIOManager;
class AssetImporterManager;
class AssetMetaDataManager;
class AssetFactoryManager;
class AssetMetaData;
class GlobalAssetRegistrySystem;
struct IntermediateAsset;
class CORE_ASSET_API AssetManager
{
  public:
    static AssetManager *GetInstance();
    ~AssetManager();

    void Initialize(AssetFactoryManager *assetFactoryManager, AssetIOManager *assetIOManager,
                    AssetImporterManager *assetImporterManager = nullptr);

    void SetAssetImporterManager(AssetImporterManager *assetImporterManager);

    AssetPtr CreateAsset(CoreAsset::EAssetType assetType, IntermediateAsset *intermediateAssetData,
                         const char *prefixAssetName);

    // filePath는 논리적경로와 일치하는 상대경로
    CoreAsset::Asset *LoadAsset(const char *filePath); // 메타데이터파일만 로드, 빈 asset생성
    bool LoadAssetRawData(CoreAsset::Asset *asset);

    // rawData 파일들이있는 경로를 설정한다.
    void SetAssetRawDataPath(const std::string &path);

    // 절대경로                                         //prefixAssetName (editor에서는 논리적 asset폴더경로가 지정됨)
    FVector<CoreAsset::Asset *> ImportAsset(const char *filePath, const char *prefixAssetName);

    // 에셋 메타데이터 저장
    bool StoreAsset(Asset *asset, const char *filePath);
    // 에셋 raw데이터 저장(texture, mesh만 해당)
    bool StoreAssetRawData(Asset *asset, const char *filePath);

    // 타입까지 검사하고 Asset 반환
    template <typename T> AssetPtr GetAsset(const char *assetName) const;
    template <typename T> AssetPtr GetAsset(AssetID id) const;

    // 타입확인 없이 Asset 반환
    AssetPtr GetAssetCommon(const char *assetName) const;
    AssetPtr GetAssetCommon(AssetID assetID) const;

  private:
    AssetPtr CreateAssetInner(/*AssetCreationInfo*/);

    // 전역테이블에 에셋을 등록한다(보통 런타임생성,로드,임포터 작업후에 수행)
    void RegisterAsset(Asset *asset);
    void InitAssetSetting(Asset *asset);

  private:
    AssetManager();

    AssetFactoryManager *mAssetFactoryManager;
    AssetIOManager *mAssetIOManager;
    AssetImporterManager *mAssetImporterManager;
    AssetMetaDataManager *mAssetMetaDataManager;
    GlobalAssetRegistrySystem *mGlobalAssetRegistrySystem;

    std::string mRawDataPath;
};

template <typename T> inline AssetPtr AssetManager::GetAsset(const char *assetName) const
{
    AssetPtr assetPtr = GetAssetCommon(assetName);

    if (assetPtr.Get() == nullptr || assetPtr.Get()->GetType() != T::GetAssetType())
        return nullptr;

    return assetPtr;
}

template <typename T> inline AssetPtr AssetManager::GetAsset(AssetID id) const
{

    AssetPtr assetPtr = GetAssetCommon(id);

    if (assetPtr.Get() == nullptr || assetPtr.Get()->GetType() != T::GetAssetType())
        return nullptr;

    return assetPtr;
}

} // namespace CoreAsset