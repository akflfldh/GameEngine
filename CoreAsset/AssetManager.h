#pragma once

#include <CoreAsset/AssetPtr.h>
#include <CoreAsset/AssetType.h>
#include <CoreAsset/CoreAssetDLLMacro.h>
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>
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
struct ImportPackage;
class IAssetDataSource;
struct AssetImportContext;
struct AssetCreationContext;
class CORE_ASSET_API AssetManager
{
  public:
    static AssetManager *GetInstance();
    ~AssetManager();

    void Initialize(AssetFactoryManager *assetFactoryManager, AssetIOManager *assetIOManager,
                    AssetImporterManager *assetImporterManager = nullptr,
                    const std::filesystem::path &editorAssetPath = "");

    void SetAssetImporterManager(AssetImporterManager *assetImporterManager);

    AssetPtr CreateAsset(CoreAsset::EAssetType assetType, const IntermediateAsset *intermediateAssetData,
                         const char *prefixAssetName, bool bEngine = false);

    AssetPtr CreateAsset(CoreAsset::EAssetType assetType, const IntermediateAsset &intermediateAssetData,
                         const char *prefixAssetName, bool bEngine = false);

    AssetPtr CreateAsset(EAssetType assetType, const IntermediateAsset *intermediateAssetData,
                         const char *registryPrefix, bool bEngine, const AssetCreationContext &creationContext);

    void SetAssetDataSource(std::unique_ptr<IAssetDataSource> assetRawDataSource);

    // filePath는 논리적경로와 일치하는 상대경로
    AssetLoadResult LoadAsset(
        const std::filesystem::path &assetPath, const std::string &logicalFolderPath,
        const AssetLoadExecutionContext &loadExecutionContext); // 메타데이터파일만 로드, 빈 asset생성

    AssetLoadResult LoadAsset(AssetID expectedAssetID, const std::string &registryName,
                              const AssetLoadExecutionContext &context);

    bool LoadAssetRawData(CoreAsset::Asset *asset);

    // rawData 파일들이있는 경로를 설정한다.
    void SetAssetRawDataPath(const std::filesystem::path &path);

    // 절대경로                                         //prefixAssetName (editor에서는 논리적 asset폴더경로가 지정됨)
    std::vector<CoreAsset::Asset *> ImportAsset(const std::filesystem::path &filePath,
                                                const CoreAsset::AssetImportContext &importContext);

    // 에셋 메타데이터 저장
    bool StoreAsset(Asset *asset, const std::filesystem::path &filePath, AssetMetaData *assetMetaData);
    // 에셋 raw데이터 저장(texture, mesh만 해당)
    bool StoreAssetRawData(Asset *asset, const std::filesystem::path &filePath);

    // 타입까지 검사하고 Asset 반환
    template <typename T> AssetPtr GetAsset(const char *assetName) const;
    template <typename T> AssetPtr GetAsset(AssetID id) const;

    // 타입확인 없이 Asset 반환
    AssetPtr GetAssetCommon(const char *assetName) const;
    AssetPtr GetAssetCommon(AssetID assetID) const;

    // 타입을 문자열로 명시 Asset반환
    AssetPtr GetAssetFromAssetName(const char *assetClassName, AssetID id) const;
    AssetPtr GetAssetFromAssetName(const char *assetClassName, const char *assetName) const;

    // 해당 에셋이 있다면 해당에셋을 반환, 없다면 기본에셋을 반환
    AssetPtr ResolveAsset(EAssetType type, AssetID id) const;

    // 기본 에셋들 등록,Get (편의)
    void RegisterDefaultAsset(EAssetType assetType, AssetID id);
    AssetID GetDefaultAssetID(EAssetType assetType) const;
    AssetPtr GetDefaultAsset(EAssetType assetType) const;
    AssetPtr GetDefaultNormalMap() const;
    AssetPtr GetDefafultDiffuseWhiteMap() const;
    AssetPtr GetDefaultMaterial() const;
    AssetID GetDefaultEngineCubeMeshID() const;

    EAssetType GetAssetTypeFromClassName(const char *className) const;
    void SetAssetTypeClassName();

    std::vector<AssetPtr> CreateBuiltInAsset();
    std::vector<AssetPtr> CreateBuiltInMesh();
    std::vector<CoreAsset::AssetPtr> CreateBuiltInStaticMeshMaterial();
    AssetPtr CreateBuiltInStaticMeshOutlineMaterial();
    std::vector<AssetPtr> CreateBuiltInTexture();
    AssetPtr CreateBuiltInDefaultNormalMap();
    AssetPtr CreateBuiltInGizmoMaterial();
    std::vector<CoreAsset::AssetPtr> CreatStaticMeshEditorUnlitMaterial();

    AssetPtr GetDefaultStaticMeshMaterial() const;

    // 0번 폰트 텍스처 , 1번 폰트
    std::vector<CoreAsset::AssetPtr> CreateBuiltInFont();

  private:
    AssetPtr CreateAssetInner(/*AssetCreationInfo*/);
    // 전역테이블에 에셋을 등록한다(보통 런타임생성,로드,임포터 작업후에 수행)
    void RegisterAsset(Asset *asset, const std::string &assetUniqueName, bool bEngine = false);
    void InitAssetSetting(Asset *asset);

    void SetAssetRawFileName(AssetMetaData *assetMetaData);

    void ProcessImportOptions(ImportPackage &importPackage);

  private:
    AssetManager();

    AssetFactoryManager *mAssetFactoryManager;
    AssetIOManager *mAssetIOManager;
    AssetImporterManager *mAssetImporterManager;
    AssetMetaDataManager *mAssetMetaDataManager;
    GlobalAssetRegistrySystem *mGlobalAssetRegistrySystem;

    std::unordered_map<EAssetType, AssetID> mDefaultAssetIDTable;
    std::unordered_map<std::string, EAssetType> mAssetClassNameTable;

    std::filesystem::path mRawDataPath;

    AssetPtr mDefaultGrayTexture;
    AssetPtr mDefaultWhiteTexture;
    AssetPtr mDefaultNormalMap;
    AssetPtr mDefaultMaterial;
    std::vector<AssetPtr> mEditorVisualizerMaterial; // Unlit
    AssetPtr mDefaultFont;
    AssetPtr mDefaultCubeMesh;

    std::filesystem::path mEditorAssetPath;
    std::unique_ptr<IAssetDataSource> mAssetRawDataSource;
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