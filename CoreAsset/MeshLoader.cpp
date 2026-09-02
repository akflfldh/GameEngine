#include "MeshLoader.h"
#include <CoreAsset/AssetFactoryManager.h>
#include <CoreAsset/AssetMetaDataType.h>
#include <CoreAsset/StaticMesh.h>

CoreAsset::MeshLoader *CoreAsset::MeshLoader::GetInstance()
{
    static MeshLoader instance;
    return &instance;
}

CoreAsset::MeshLoader::MeshLoader() {}
CoreAsset::MeshLoader::~MeshLoader() {}
bool CoreAsset::MeshLoader::LoadAssetFile(EAssetType assetType, Arch &arch, AssetFactoryManager *assetFactoryManage,
                                          Asset *&oAsset, std::unique_ptr<AssetMetaData> &oAssetMetaDataPtr)
{

    MeshCommonHeader meshCommonHeader;
    meshCommonHeader.Serialize(arch);

    std::unique_ptr<MeshMetaData> meshMetaData = std::make_unique<MeshMetaData>();

    oAssetMetaDataPtr = std::move(meshMetaData);

    // 그 후 빈에셋(Texture)생성후 Serialize수행
    AssetFactoryManager *assetFactoryManager = AssetFactoryManager::GetInstance();

    if (assetType == EAssetType::eStaticMesh)
        oAsset = assetFactoryManager->CreateEmptyAsset(EAssetType::eStaticMesh);
    else if (assetType == EAssetType::eSkinningMesh)
        oAsset = assetFactoryManager->CreateEmptyAsset(EAssetType::eSkinningMesh);

    if (oAsset != nullptr)
        oAsset->Serialize(arch);

    return true;
}

bool CoreAsset::MeshLoader::LoadAssetRawFile(Arch &arch, Asset *asset)
{

    Mesh *mesh = static_cast<Mesh *>(asset);

    std::vector<MeshIndexType> &indexVector = mesh->GetMeshIndexVector();

    arch << indexVector;

    switch (asset->GetType())
    {
    case EAssetType::eStaticMesh:

    {
        StaticMesh *staticMesh = static_cast<StaticMesh *>(mesh);
        std::vector<StaticVertex> vertexVector;

        //   std::vector<StaticVertex> &vertexVector = staticMesh->GetVertexVector();
        arch << vertexVector;
        staticMesh->SetVertexVector(std::move(vertexVector));
    }
    break;

    case EAssetType::eSkinningMesh:

        break;
    }

    return true;
}
